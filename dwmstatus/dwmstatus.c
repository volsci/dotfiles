/*
 * Copy me if you can.
 * by 20h
 */

#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <X11/Xlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>

char *tzbris = "Australia/Brisbane";

static Display *dpy;

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	ret = malloc(++len);
	if (ret == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

void
settz(char *tzname)
{
	setenv("TZ", tzname, 1);
}

char *
mktimes(char *fmt, char *tzname)
{
	char buf[129];
	time_t tim;
	struct tm *timtm;

	settz(tzname);
	tim = time(NULL);
	timtm = localtime(&tim);
	if (timtm == NULL)
		return smprintf("");

	if (!strftime(buf, sizeof(buf)-1, fmt, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		return smprintf("");
	}

	return smprintf("%s", buf);
}

void
setstatus(char *str)
{
	XStoreName(dpy, DefaultRootWindow(dpy), str);
	XSync(dpy, False);
}

char *
loadavg(void)
{
	double avgs[3];

	if (getloadavg(avgs, 3) < 0)
		return smprintf("");

	return smprintf("%.2f %.2f %.2f", avgs[0], avgs[1], avgs[2]);
}

char *
readfile(char *base, char *file)
{
	char *path, line[513];
	FILE *fd;

	memset(line, 0, sizeof(line));

	path = smprintf("%s/%s", base, file);
	fd = fopen(path, "r");
	free(path);
	if (fd == NULL)
		return NULL;

	if (fgets(line, sizeof(line)-1, fd) == NULL)
		return NULL;
	fclose(fd);

	return smprintf("%s", line);
}

char *
getbattery(char *base)
{
	char *co, status;
	int descap, remcap;

	descap = -1;
	remcap = -1;

	co = readfile(base, "present");
	if (co == NULL)
		return smprintf("");
	if (co[0] != '1') {
		free(co);
		return smprintf("not present");
	}
	free(co);

	co = readfile(base, "charge_full_design");
	if (co == NULL) {
		co = readfile(base, "energy_full_design");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &descap);
	free(co);

	co = readfile(base, "charge_now");
	if (co == NULL) {
		co = readfile(base, "energy_now");
		if (co == NULL)
			return smprintf("");
	}
	sscanf(co, "%d", &remcap);
	free(co);

	co = readfile(base, "status");
	if (!strncmp(co, "Discharging", 11)) {
		status = '-';
	} else if(!strncmp(co, "Charging", 8)) {
		status = '+';
	} else {
		status = '?';
	}

	if (remcap < 0 || descap < 0)
		return smprintf("invalid");

	return smprintf("%.0f%%%c", ((float)remcap / (float)descap) * 100, status);
}

char *
gettemperature(char *base, char *sensor)
{
	char *co;

	co = readfile(base, sensor);
	if (co == NULL)
		return smprintf("");
	return smprintf("%02.0f°C", atof(co) / 1000);
}


void 
alsavolume(char* volstr) {
	long min,max,volleft,volright,volavg;
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	const char *card = "default";
	const char *selem_name = "Master";
		    
	snd_mixer_open(&handle,0);
	snd_mixer_attach(handle,card);
	snd_mixer_selem_register(handle,NULL,NULL);
	snd_mixer_load(handle);
				    
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid,0);
	snd_mixer_selem_id_set_name(sid,selem_name);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle,sid);
						    
	snd_mixer_selem_get_playback_volume_range(elem,&min,&max);
	snd_mixer_selem_get_playback_volume(elem,0,&volleft);
	snd_mixer_selem_get_playback_volume(elem,1,&volright);
	volavg = (volleft+volright)/2;
	sprintf(volstr,"%ld",((volavg-min)*100)/(max-min));
}

int
main(void)
{
	char *status;
	char *bat;
	char *tmbris;
	char *t0;
	char volm[40];

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(5)) {
		bat = getbattery("/sys/class/power_supply/BAT0");
		tmbris = mktimes("Week %V %a, %d %b %I:%M %p", tzbris);
		alsavolume(volm);	
		t0 = gettemperature("/sys/devices/virtual/hwmon/hwmon0", "temp1_input");

		status = smprintf(" %s | %s | %s%% | %s",
				t0, bat, volm, tmbris);
		setstatus(status);

		free(t0);
		free(bat);
		free(tmbris);
		free(status);
	}

	XCloseDisplay(dpy);

	return 0;
}

