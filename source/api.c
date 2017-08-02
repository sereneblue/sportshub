#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <3ds.h>
#include "api.h"
#include "cJSON.h"

lge_vec_t leagues;

Result http_get(const char *url, char ** response_data) {
	Result ret=0;
	httpcContext context;
	char *newurl=NULL;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
	u8 *buf, *lastbuf;

	do {
		ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36");

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcAddRequestHeaderField(&context, "Connection", "Keep-Alive");

		ret = httpcBeginRequest(&context);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(&context);
				return -1;
			}
			ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
			url = newurl; // Change pointer to the url that we just learned
			httpcCloseContext(&context); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -2;
	}

	// This relies on an optional Content-Length header and may be 0
	ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return ret;
	}

	// Start with a single page buffer
	buf = (u8*)malloc(0x1000);
	if(buf==NULL){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -1;
	}

	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize;
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){
					httpcCloseContext(&context);
					free(lastbuf);
					if(newurl!=NULL) free(newurl);
					return -1;
				}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		free(buf);
		return -1;
	}

	// Resize the buffer back down to our actual final size
	lastbuf = buf;
	buf = realloc(buf, size);
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
		if(newurl!=NULL) free(newurl);
		return -1;
	}
	httpcCloseContext(&context);
	if (newurl!=NULL) free(newurl);

	*response_data = malloc((strlen(buf) + 1) * sizeof(char));
	strcpy(*response_data, buf);
	free(buf);
	return 0;
}

void extract_games(cJSON *item) {
	vec_init(&leagues);
	cJSON *tournaments = cJSON_GetObjectItem(item, "tournaments");

	if (tournaments) {
		short count = cJSON_GetArraySize(tournaments);
		league l[count];

		for (short i = 0; i < count; i++) {
			cJSON *subitem = cJSON_GetArrayItem(tournaments, i);
			cJSON *tournament = cJSON_GetObjectItem(subitem, "tournament");
			cJSON *events = cJSON_GetObjectItem(subitem, "events");
			l[i].name = cJSON_GetObjectItem(tournament, "name")->valuestring;
			vec_init(&l[i].gms);

			for (short j = 0; j < cJSON_GetArraySize(events); j++) {
				cJSON *event = cJSON_GetArrayItem(events, j);
				cJSON *home = cJSON_GetObjectItem(event, "homeTeam");
				cJSON *away = cJSON_GetObjectItem(event, "awayTeam");
				cJSON *homeScore = cJSON_GetObjectItem(event, "homeScore");
				cJSON *awayScore = cJSON_GetObjectItem(event, "awayScore");
				char *status = cJSON_GetObjectItem(event, "statusDescription")->valuestring;

				game gm;
				gm.home = cJSON_GetObjectItem(home, "name")->valuestring;
				gm.away = cJSON_GetObjectItem(away, "name")->valuestring;
				get_date(cJSON_GetObjectItem(event, "startTimestamp")->valueint, gm.date);
				gm.home_score = 0;
				gm.away_score = 0;
				gm.winner = cJSON_GetObjectItem(event, "winnerCode")->valueint;

				if (strcmp(status, "FT") == 0) {
					gm.status = "FT";
				} else if (strcmp(status, "-") == 0) {
					gm.status = cJSON_GetObjectItem(event, "startTime")->valuestring;
				} else if (cJSON_GetObjectItem(event, "lastPeriod")) {
					gm.status = cJSON_GetObjectItem(event, "lastPeriod")->valuestring;
				} else {
					gm.status = "";
				}

				if (cJSON_GetObjectItem(homeScore, "current")) {
					gm.home_score = cJSON_GetObjectItem(homeScore, "current")->valueint;
					gm.away_score = cJSON_GetObjectItem(awayScore, "current")->valueint;
				}
				vec_push(&l[i].gms, gm);
			}
			vec_push(&leagues, l[i]);
		}
	}
}

void get_scores(char *sport) {
	char *response;
	char *url;
	char date[11];

	time_t t;
	struct tm *today;
	time(&t);
	today = localtime(&t);
	strftime(date, 11, "%F", today);

	asprintf(&url, "https://www.sofascore.com/%s/usa/%s/json", sport, date);

	Result ret=0;

	ret = http_get(url, &response);

	cJSON *root = cJSON_Parse(response);
	cJSON *sportItem = cJSON_GetObjectItem(root, "sportItem");
	extract_games(sportItem);
}

void get_date(int seconds, char *date) {
	time_t t = seconds;
	struct tm *info = localtime(&t);

	strftime(date, 6,"%m/%d", info);
}