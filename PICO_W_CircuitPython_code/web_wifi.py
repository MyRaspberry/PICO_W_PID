# wifi
import os
import time
#from adafruit_datetime import  datetime
import socketpool
from ipaddress import ip_address
import wifi
from adafruit_httpserver import Server, Request, Response, Redirect, GET, POST
import micropython
import gc # micropython garbage collection # use gc.mem_free() # use gc.collect

from pico_w_io import get_datas, get_pids, get_PV, get_SP, set_SP, get_OUT, set_OUT, get_pid_details, get_pid_mode
datas="Ain datas from pico_w_io"
pids="Pid data from pico_w_io"
isPV=50.0
isSP=50.0
isOUT=0.0


DIAG = True  # False
DIAG = bool(os.getenv('DIAG')) # ______________________________ now get from settings.toml

def dp(line=" ", ende="\n"):
    if DIAG:
        print(line, end=ende)


THIS_REVISION = os.getenv('THIS_REVISION')
THIS_OS = os.getenv('THIS_OS')

WIFI_SSID = os.getenv('WIFI_SSID')
WIFI_PASSWORD = os.getenv('WIFI_PASSWORD')

WIFI_IP = os.getenv('WIFI_IP')


HTML_STYLE = """
<style>
    html {font-family: "Times New Roman", Times, serif; background-color: lightgreen;
    display:inline-block; margin: 0px auto; text-align: center;}
    h1{color: deeppink; width: 200; word-wrap: break-word; padding: 2vh; font-size: 35px;}
    p{font-size: 1.5rem; width: 200; word-wrap: break-word;}
    form{font-size: 2rem; }
    input[type=number]{font-size: 2rem;}
    .button{font-family: {font_family};display: inline-block;
    background-color: black; border: none;
    border-radius: 4px; color: white; padding: 16px 40px;
    text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
    p.dotted {margin: auto; width: 75%; font-size: 25px; text-align: center;}
</style>
"""

def html_pid_faceplate_svg():
    # use OUT, PV, SP
    isOUT = get_OUT()
    isSP = get_SP()
    isPV = get_PV()
    isMODE = get_pid_mode()

    svg  =  "<div><table align=\"center\">"
    svg  =  "<td width=\"10%\">&nbsp;&nbsp;<br></td><td width=\"10%\">"
    svg +=  "<tr><td>\n"
    scopew = 640
    svgw = scopew+20
    scopeh = 450
    svgh = scopeh+30
    svg += "<svg width=\"{:d}\" height=\"{:d}\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">".format(svgw, svgh)
    svg += "<desc>show PID faceplate </desc>"
    svg += ("<!-- grey box with 3D style blue border-->")
    svg += "<rect x=\"1\" y=\"1\" width=\"{:d}\" height=\"{:d}\"  stroke=\"#000088\" stroke-width=\"1\" />".format(svgw, svgh)
    svg += "<rect x=\"2\" y=\"2\" width=\"{:d}\" height=\"{:d}\"  stroke=\"#0000aa\" stroke-width=\"1\" />".format((svgw-4),(svgh-4))
    svg += "<rect x=\"3\" y=\"3\" width=\"{:d}\" height=\"{:d}\"  fill=\"#FFF8DC\" stroke=\"#0000ff\" stroke-width=\"1\" />".format((svgw-6),(svgh-6))
    svg += ("<g id=\"faceplate\" style=\"visibility:visible; stroke-width:2; stroke=\"#000000\"; font-family=\"Verdana\"; font-size=\"16\";\" > ")
    # bar graph
    #svg += ("<!-- cyan OUTPUT bar-->");
    svg += ("<rect x=\"50\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#00ffff\" height=\"402\"/>")
    svg += ("<rect x=\"52\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"")
    svg += str(400 - int(4*isOUT))
    svg += ("\"/>")

    #svg += ("<!-- OUT value + EGU --> ") #font-family=\"Verdana\" font-size=\"14\" stroke=\"#000000\"
    svg += ("<rect x=\"100\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />")
    svg += ("<text x=\"150\" y=\"76\"  text-anchor=\"middle\">")
    svg += f"OUT: {isOUT:,.1f}"
    svg += ("</text>")
    svg += ("<text x=\"150\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>")

    #svg += ("<!-- yellow PV bar-->")
    svg += ("<rect x=\"200\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffff00\" height=\"402\"/>")
    svg += ("<rect x=\"202\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"")
    svg += str(400 - int(4*isPV))
    svg += ("\"/>")

    #svg += ("<!-- PV value + EGU --> ")
    svg += ("<rect x=\"250\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />  <!-- PV text background rectangle -->")  # use PV in EGU
    svg += ("<text x=\"300\" y=\"76\" text-anchor=\"middle\">")
    svg += f"PV: {isPV:,.1f}"
    svg += ("</text>")
    svg += ("<text x=\"300\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>")

    #svg += ("<!-- white SP bar-->");
    svg += ("<rect x=\"350\" y=\"40\" width=\"40\" stroke=\"#000000\" fill=\"#ffffff\" height=\"402\"/>")
    svg += ("<rect x=\"352\" y=\"42\" width=\"36\" stroke=\"#777777\" fill=\"#777777\" height=\"")
    svg += str(400 - int(4*isSP))
    svg += ("\"/>")

    #svg += ("<!-- SP value + EGU --> ")
    svg += ("<rect x=\"400\" y=\"61\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />")
    #<!-- PV text background rectangle -->
    svg += ("<text x=\"450\" y=\"76\" text-anchor=\"middle\">")
    svg += f"SP: {isSP:,.1f}"
    svg += ("</text>")
    svg += ("<text x=\"450\" y=\"91\" text-anchor=\"middle\">[ pct ]</text>")

    #svg += ("<!-- MODE --> ")
    svg += ("<rect x=\"500\" y=\"161\" width=\"93\"  height=\"38\" stroke=\"#ffffff\" fill=\"#ffffff\" />")
    #<!-- PV text background rectangle -->
    svg += ("<text x=\"550\" y=\"176\" text-anchor=\"middle\">")
    svg += ("MODE:</text>")
    svg += ("<text x=\"550\" y=\"191\" font-size=\"18\" text-anchor=\"middle\">")

    if isMODE :
        svg += ("AUTO")
    else:
        svg += ("MAN")

    svg += ("</text>")

    svg += ("</g>")
    svg += ("</svg>\n")

    svg +=  "</td><td width=\"10%\">&nbsp;&nbsp;<br></td><td width=\"10%\"></tr></table></div>\n"

    return svg

HTML_INDEX = """
<!DOCTYPE html><html><head><title>KLL Pico W Web Server</title>
    {HTML_STYLE}
    <meta http-equiv="refresh" content="30">
    </head><body>
        <h1>KLL Pico W Web Server</h1>
        <h2> from Circuit Python {THIS_OS} </h2>
        <img src="https://www.raspberrypi.com/documentation/microcontrollers/images/pico-pinout.svg" >
        <hr>
        <H2>{datas}</H2>
        <hr><p> PID </p>
        <H2>{pids}</H2>
        <p>{get_pid_details}</p>
        {SVG_FACE}
        <hr>

        <table style="width:100%">
            <tr>
                <th><H2>operation:</H2></th>
                <th><H2>PID</H2></th>
            </tr>
            <tr>
                <td>
                    <form action="/form/OUT" method="post">
                        OUT: <input type=number value={isOUT:,.2f} step=0.1 id="OUT" size="8" name="OUT" min="0" max="100" />
                    </form>
                </td>
                <td>
                    <form action="/form/SP" method="post">
                        SP: <input type=number value={isSP:,.2f} step=0.1 id="SP" name="SP" size="8" min="0" max="100" />
                    </form>
                </td>
            </tr>
            <tr>
                <td>
                    <p>and use MANUAL MODE</p>
                </td>
                <td>
                    <p>and use AUTO MODE</p>
                </td>
            </tr>
        </table>

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
                    <p>page auto refresh 30sec</p>
                </td>
                <td>
                    <p>made 10.12.2023</p>
                </td>
            </tr>
        </table>


        </p>
    </body></html>
"""

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
    # ____________________________________________________ make a WEB SERVER
    server = Server(pool) #, debug=True)

    @server.route("/")
    def base(request):  # pylint: disable=unused-argument
        dp("\nwww served dynamic index.html")
        return Response(request,
            HTML_INDEX.format(
                HTML_STYLE=HTML_STYLE,
                THIS_OS=THIS_OS,
                datas=get_datas(),
                pids=get_pids(),
                get_pid_details=get_pid_details(),
                SVG_FACE=html_pid_faceplate_svg(),
                isSP=get_SP(),
                isOUT=get_OUT(),
                THIS_REVISION=THIS_REVISION
                ),
                content_type='text/html'
            )


    @server.route("/form/SP", [GET, POST])
    def form(request):
        if request.method == POST:
            posted_SP_value = request.form_data.get("SP")
            dp(f"\nwww SP input: {posted_SP_value}")
            try:
                newSP = float(posted_SP_value)
                set_SP(newSP) # and change MODE to AUTO
            except ValueError:
                print("Not a Digit")

        return Redirect(request, "/" ) # _________________ back to index page

    @server.route("/form/OUT", [GET, POST])
    def form(request):
        if request.method == POST:
            posted_OUT_value = request.form_data.get("OUT")
            dp(f"\nwww OUT input: {posted_OUT_value}")
            try:
                newOUT = float(posted_OUT_value)
                set_OUT(newOUT) # and change MODE to MAN
            except ValueError:
                print("Not a Digit")

        return Redirect(request, "/" ) # _________________ back to index page


    server.start(str(wifi.radio.ipv4_address)) # _________ startup the server

def run_webserver() :
    global server
    try:
        #checkT = time.monotonic()
        server.poll()
        #dp("\n___ server poll: {:>5.3f} sec ".format((time.monotonic() - checkT)))

    except OSError:
        print("ERROR server poll")
