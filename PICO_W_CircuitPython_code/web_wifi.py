# wifi
import gc # micropython garbage collection # use gc.mem_free() # use gc.collect
Imp=(f"\nFREE MEM report from web_wifi.py after imports\n+ import gc {gc.mem_free()}\n")
import os
Imp+=(f"+ import os {gc.mem_free()}\n")
THIS_OS = f" This is a {os.uname().machine} with CP{os.uname().version} "
import time
Imp+=(f"+ import time {gc.mem_free()}\n")
#   from adafruit_datetime import  datetime
#   Imp+=(f"+ from adafruit_datetime import  datetime {gc.mem_free()}\n")
import rtc
Imp+=(f"+ import rtc {gc.mem_free()}\n")
import adafruit_ntp # V1.0.2 b use NTP time to set PICO W RTC
Imp+=(f"+ import adafruit_ntp {gc.mem_free()}\n")
import socketpool
Imp+=(f"+ import socketpool {gc.mem_free()}\n")
from ipaddress import ip_address
Imp+=(f"+ from ipaddress import ip_address {gc.mem_free()}\n")
import wifi
Imp+=(f"+ import wifi {gc.mem_free()}\n")
from adafruit_httpserver import Server, Request, Response, Redirect, GET, POST
Imp+=(f"+ from adafruit_httpserver import Server, Request, Response, GET, POST {gc.mem_free()}\n")
import micropython
Imp+=(f"+ import micropython {gc.mem_free()}\n")
import microcontroller # for board reboot
Imp+=(f"+ import microcontroller {gc.mem_free()}\n")

from pico_w_io import get_datas, get_pids, get_PV, get_SP, set_SP, get_OUT, set_OUT, get_pid_details, get_pid_mode
datas="Ain data from pico_w_io"
pids="Pid data from pico_w_io"
isPV=0.0
isSP=0.0
isOUT=0.0

from mqtt import mqtt_connect, get_mqtts # _____________________ file mqtt.py

# ________________________________________________________ expect file tools.py
from tools import DIAG,DIAGM , dp, check_mem

if ( DIAGM ) : print(Imp)
del Imp # ________________________________________________ variable needed for boot only

THIS_REVISION = os.getenv('THIS_REVISION')
#THIS_OS = os.getenv('THIS_OS')

WIFI_SSID = os.getenv('WIFI_SSID')
WIFI_PASSWORD = os.getenv('WIFI_PASSWORD')

WIFI_IP = os.getenv('WIFI_IP')

TZ_OFFSET = os.getenv('TZ_OFFSET') # for NTP to RTC
useNTP = bool(os.getenv('useNTP'))

def get_network_time():
    if useNTP :
        try:
            print("___ get NTP to RTC")
            ntp = adafruit_ntp.NTP(pool, tz_offset=TZ_OFFSET)
            rtc.RTC().datetime = ntp.datetime # NOTE: This changes the system time
        except:
            print("failed")

def time_now(): # ______________________________________ use time and NOT datetime
    if useNTP :
        now = time.localtime()
        nows = f"{now.tm_year}-{now.tm_mon:02}-{now.tm_mday:02} {now.tm_hour:02}:{now.tm_min:02}:{now.tm_sec:02}"
        return nows
    else:
        return ""


# ______________________________ some SVG scale and formatting functions used in HTML_PID.format()
def get_svgw(): # SVG width
    return (640+20)

def get_svgh(): # SVG height
    return (450+30)

def get_OUTh():
    isOUT=get_OUT()
    return  (400 - int(4*isOUT))

def get_PVh():
    isPV=get_PV()
    return (400 - int(4*isPV))

def get_SPh():
    isSP=get_SP()
    return (400 - int(4*isSP))


def get_MODEs():
    isMODE = get_pid_mode()
    if isMODE :
        return "AUTO"
    else:
        return "MAN"


if ( DIAG ) : print(f"+ before declare HTML's {gc.mem_free()}")

# ______________________________ at the HTML STYLE section i had to escape the { , } by {{ , }}
HTML_INDEX = """
<!DOCTYPE html><html><head>
<title>KLL engineering</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
<style>
body {{font-family: "Times New Roman", Times, serif; background-color: lightgreen;
display:inline-block; margin: 0px auto; text-align: center;}}
h1 {{color: deeppink; word-wrap: break-word; padding: 1vh; font-size: 30px;}}
p {{font-size: 1.5rem; word-wrap: break-word;}}
p.dotted {{margin: auto; width: 75%; font-size: 25px; text-align: center;}}
form {{font-size: 2rem; }}
input[type=number] {{font-size: 2rem;}}
</style>
</head><body>
<h1>{THIS_OS} Web-server</h1>
<img src="https://www.raspberrypi.com/documentation/microcontrollers/images/picow-pinout.svg" >
<hr>
<a href="/data" target="_blank" ><b>data</b></a>
<hr>
<table style="width:100%">
<tr>
<th>
<p><a href="http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=227" target="_blank" >
<b>kll engineering blog</b></a></p>
</th>
<th>
<p>rev: {THIS_REVISION}</p>
</th>
</tr>
</table>
<hr>
</body></html>
"""

HTML_PID = """
<!DOCTYPE html><html><head>
<title>KLL engineering Pico W</title>
<link rel="icon" type="image/x-icon" href="favicon.ico">
<style>
body {{font-family: "Times New Roman", Times, serif; background-color: lightgreen;
display:inline-block; margin: 0px auto; text-align: center;}}
h1 {{color: deeppink; word-wrap: break-word; padding: 1vh; font-size: 30px;}}
p {{font-size: 1.5rem; word-wrap: break-word;}}
p.dotted {{margin: auto; width: 75%; font-size: 25px; text-align: center;}}
form {{font-size: 2rem; }}
input[type=number] {{font-size: 2rem;}}
</style>
<meta http-equiv="refresh" content="30">
</head><body>
<hr>
<H1> Analog I/O </H1>
<H2>{datas}</H2>
<hr>
<H1> PID </H1>
<H2>{pids}</H2>
<H3>{get_pid_details}</H3>
<table align=\"center\" style="width:100%">
<tr>
<td width=\"10%\"></td>
<td>\n
<svg width=\"{svgw}\" height=\"{svgh}\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">
<desc>show PID faceplate </desc>
<!-- grey box with 3D style blue border-->")
<rect x=\"1\" y=\"1\" width=\"{svgw}\" height=\"{svgh}\"  stroke=\"#000088\" stroke-width=\"1\" />
<rect x=\"2\" y=\"2\" width=\"{svgw4}\" height=\"{svgh4}\"  stroke=\"#0000aa\" stroke-width=\"1\" />
<rect x=\"3\" y=\"3\" width=\"{svgw6}\" height=\"{svgh6}\"  fill=\"#FFF8DC\" stroke=\"#0000ff\" stroke-width=\"1\" />
<g id=\"faceplate\" style=\"visibility:visible; stroke-width:2; stroke=\"#000000\"; font-family=\"Verdana\"; font-size=\"16\";\" >
<!-- cyan OUTPUT bar-->
<rect x=\"50\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#00ffff\" height=\"402\"/>
<rect x=\"52\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"{OUTh}\"/>
<rect x=\"100\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />
<text x=\"150\" y=\"76\"  text-anchor=\"middle\">OUT: {isOUT:,.1f}</text>
<text x=\"150\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>
<!-- yellow PV bar-->
<rect x=\"200\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffff00\" height=\"402\"/>
<rect x=\"202\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"{PVh}\"/>
<!-- PV value + EGU -->
<rect x=\"250\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />
<!-- PV text background rectangle -->
<text x=\"300\" y=\"76\" text-anchor=\"middle\">PV: {isPV:,.1f}</text>
<text x=\"300\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>
<!-- white SP bar-->
<rect x=\"350\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffffff\" height=\"402\"/>
<rect x=\"352\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"{SPh}\"/>
<!-- SP value + EGU -->
<rect x=\"400\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />
<!-- PV text background rectangle -->
<text x=\"450\" y=\"76\" text-anchor=\"middle\">SP: {isSP:,.1f}</text>
<text x=\"450\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>
<!-- MODE -->
<rect x=\"500\" y=\"161\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />
<!-- PV text background rectangle -->
<text x=\"550\" y=\"176\" text-anchor=\"middle\">
"MODE:</text>
<text x=\"550\" y=\"191\" font-size=\"18\" text-anchor=\"middle\">{MODEs}</text>
</g>
</svg>\n
</td>
<td width=\"10%\"></td>
</tr>
</table>
<table style="width:100%">
<tr>
<td>
<form action="/data/form/OUT" method="post">
OUT: <input type=number value={isOUT:,.2f} step=0.1 id="OUT" size="8" name="OUT" min="0" max="100" />
</form>
</td>
<td>
<form action="/data/form/SP" method="post">
SP: <input type=number value={isSP:,.2f} step=0.1 id="SP" name="SP" size="8" min="0" max="100" />
</form>
</td>
</tr>
<tr>
<td>
<p>and go MANUAL MODE</p>
</td>
<td>
<p>and go AUTO MODE</p>
</td>
</tr>
</table>
<hr>
<H1> last MQTT to broker:</H1>
<p>{mqtts}</p>
<hr>
<table style="width:100%">
<tr>
<th>
<p><a href="http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=227" target="_blank" >
<b>kll engineering blog</b></a></p>
</th>
<th>
<p>rev: {THIS_REVISION}</p>
</th>
</tr>
<tr>
<td>
<p>{tnows}</p>
</td>
<td>
<p>page auto refresh 30sec</p>
</td>
</tr>
</table>
<hr>
</body></html>
"""

if ( DIAG ) : print(f"+ after declare HTML's {gc.mem_free()}")

def setup_webserver() :
    global server, pool, datas, pids
    dp("\n\nwww PICO W: Hello World! start webserver (STA)\n")
    dp("www Connecting to router {:s} OR CHECK THE 'settings.toml' FILE".format( WIFI_SSID) )
    wifi.radio.set_ipv4_address( # _______________________ fixIP ( requires > CP 8.0.0 beta 4 )
        ipv4=ip_address(WIFI_IP),
        netmask=ip_address("255.0.0.0"),
        gateway=ip_address("192.168.1.1"),
        ipv4_dns=ip_address("192.168.1.1"),
    )
    wifi.radio.connect(WIFI_SSID, WIFI_PASSWORD)
    dp("www Connected to {:s}".format( WIFI_SSID) )
    dp("www Listening on http://{:s}:80 ".format(str(wifi.radio.ipv4_address)) )

    pool = socketpool.SocketPool(wifi.radio)

    get_network_time() # _________________________________ get network time to RTC
    dp(time_now())

    # ____________________________________________________ make a WEB SERVER
    server = Server(pool, "/static") #, debug=True)

    @server.route("/")
    def base(request):  # pylint: disable=unused-argument
        dp("\nwww served dynamic index.html")
        check_mem(info = "serve /",prints=DIAGM,coll=True)
        return Response(request,
            HTML_INDEX.format(
                THIS_OS=THIS_OS,
                THIS_REVISION=THIS_REVISION,
                ),
                content_type='text/html'
            )

    @server.route("/data")
    def data(request):  # pylint: disable=unused-argument
        dp("\nwww served dynamic data.html")
        check_mem(info = "serve /data",prints=DIAGM,coll=True)
        return Response(request,
            HTML_PID.format(
                datas=get_datas(),
                pids=get_pids(),
                get_pid_details=get_pid_details(),
                svgw=get_svgw(),
                svgh=get_svgh(),
                svgw4=get_svgw()-4,
                svgh4=get_svgh()-4,
                svgw6=get_svgw()-6,
                svgh6=get_svgh()-6,
                isPV=get_PV(),
                isSP=get_SP(),
                isOUT=get_OUT(),
                OUTh = get_OUTh(),
                PVh =get_PVh(),
                SPh=get_SPh(),
                isMODE = get_pid_mode(),
                MODEs=get_MODEs(),
                tnows=time_now(),
                mqtts=get_mqtts(),
                THIS_REVISION=THIS_REVISION,
                ),
                content_type='text/html'
            )

    @server.route("/data/form/SP", [GET, POST])
    def form(request):
        check_mem(info = "serve /data/form/SP",prints=False,coll=True)
        if request.method == POST:
            posted_SP_value = request.form_data.get("SP")
            dp(f"\nwww SP input: {posted_SP_value}")
            try:
                newSP = float(posted_SP_value)
                set_SP(newSP) # and change MODE to AUTO
            except ValueError:
                print("Not a Digit")

        return Redirect(request, "/data" ) # _________________ back to index page

    @server.route("/data/form/OUT", [GET, POST])
    def form(request):
        check_mem(info = "serve /data/form/OUT",prints=False,coll=True)
        if request.method == POST:
            posted_OUT_value = request.form_data.get("OUT")
            dp(f"\nwww OUT input: {posted_OUT_value}")
            try:
                newOUT = float(posted_OUT_value)
                set_OUT(newOUT) # and change MODE to MAN
            except ValueError:
                print("Not a Digit")

        return Redirect(request, "/data" ) # _________________ back to index page


    server.start(str(wifi.radio.ipv4_address)) # _________ startup the server

    mqtt_connect(pool) # ______________________________________ above MQTT start


def run_webserver() :
    global server
    try:
        #checkT = time.monotonic()
        server.poll()
        #dp("\n___ server poll: {:>5.3f} sec ".format((time.monotonic() - checkT))) # 0.000 or 0.004 ?

    except OSError:
        print("ERROR server poll")
        # _________________________________________________ here do a reboot
        microcontroller.reset()
