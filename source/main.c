#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
Result http_post(const char* url, const char* data)
{
    Result ret=0;
    httpcContext context;
    char *newurl=NULL;
    u32 statuscode=0;
    u32 contentsize=0, readsize=0, size=0;
    u8 *buf, *lastbuf;

    do {
        ret = httpcOpenContext(&context, HTTPC_METHOD_POST, url, 0);

        ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
        
        ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);

        ret = httpcAddRequestHeaderField(&context, "User-Agent", "httpc-example/1.0.0");

        ret = httpcAddRequestHeaderField(&context, "Content-Type", "application/json");
        
        ret = httpcAddPostDataRaw(&context, (u32*)data, strlen(data));

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
            url = newurl;
            httpcCloseContext(&context);
        }
    } while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

    if(statuscode!=200){
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return -2;
    }

    ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
    if(ret!=0){
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return ret;
    }

    buf = (u8*)malloc(0x1000);
    if(buf==NULL){
        httpcCloseContext(&context);
        if(newurl!=NULL) free(newurl);
        return -1;
    }

    do {
        ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
        size += readsize;
        if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
                lastbuf = buf;
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

    lastbuf = buf;
    buf = realloc(buf, size);
    if(buf==NULL){
        httpcCloseContext(&context);
        free(lastbuf);
        if(newurl!=NULL) free(newurl);
        return -1;
    }
    httpcCloseContext(&context);
    free(buf);
    if (newurl!=NULL) free(newurl);

    return 0;
}
int main(int argc, char **argv)
{
	char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};

	gfxInitDefault();

	consoleInit(GFX_TOP, NULL);
    httpcInit(4 * 1024 * 1024);
	u32 kDownOld = 0, kUpOld = 0;
    static SwkbdState swkbd;
    swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 21);
    swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
    swkbdSetNumpadKeys(&swkbd, L'.', L':');
    swkbdSetHintText(&swkbd, "000.000.000.000:00000");
    char ipBuf[21];
    swkbdInputText(&swkbd, ipBuf, sizeof(ipBuf));
    char baseAddl[32] = "http://";
    strncat(baseAddl, ipBuf, 21);
    strcat(baseAddl, "/c?,");
    printf("Base URL:\n%s", baseAddl);
    gfxFlushBuffers();
	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

        u32 kDown = hidKeysDown();
		u32 kUp = hidKeysUp();
        
        circlePosition pos;
        hidCircleRead(&pos);
        
        touchPosition touch;
        hidTouchRead(&touch);
		if ((kDown & KEY_START) && (kDown & KEY_SELECT)) break; // break in order to return to hbmenu

		//Do the keys printing only if keys have changed
		if (kDown != kDownOld || kUp != kUpOld)
		{
            char addl[202];
            strncpy(addl, baseAddl, 32);
			int i;
			for (i = 0; i < 32; i++)
			{
				if (kDown & BIT(i)) {
                    strcat(addl, "D");
                    strncat(addl, keysNames[i], 14);
                    strcat(addl, ",");
                }
                if (kUp & BIT(i)) {
                    strcat(addl, "U");
                    strncat(addl, keysNames[i], 14);
                    strcat(addl, ",");
                }
			}
            char posx[4];
            itoa(pos.dx, posx, 10);
            char posy[4];
            itoa(pos.dy, posy, 10);
            strncat(addl, posx, 4);
            strcat(addl, ",");
            strncat(addl, posy, 4);
            strcat(addl, ",");
            char touchx[4];
            itoa(touch.px, touchx, 10);
            char touchy[4];
            itoa(touch.py, touchy, 10);
            strncat(addl, touchx, 4);
            strcat(addl, ",");
            strncat(addl, touchy, 4);
            if (strncmp(addl, baseAddl, 29) != 0) {
                http_post(addl, "");
            }
        } else {
            char addl[202];
            strncpy(addl, baseAddl, 32);
            char posx[4];
            itoa(pos.dx, posx, 10);
            char posy[4];
            itoa(pos.dy, posy, 10);
            strncat(addl, posx, 4);
            strcat(addl, ",");
            strncat(addl, posy, 4);
            strcat(addl, ",");
            char touchx[4];
            itoa(touch.px, touchx, 10);
            char touchy[4];
            itoa(touch.py, touchy, 10);
            strncat(addl, touchx, 4);
            strcat(addl, ",");
            strncat(addl, touchy, 4);
            strcat(addl, ",");
            http_post(addl, "");
        }

		//Set keys old values for the next frame
		kDownOld = kDown;
		kUpOld = kUp;
	}
    httpcExit();
	gfxExit();
	return 0;
}
