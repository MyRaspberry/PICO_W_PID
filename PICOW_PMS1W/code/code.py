# ___________________________________________________ KLL 2022-12-15
# ___________________________________________________ PMS1W
# ___________________________________________________ see settings.toml
# THIS FILE 'pms1w' IS NOT PART OF THIS code.py PROJECT
# it is an extra application
# but if you stop from REPL with [ctrl][c]
# and load pms1w.py OSCI from REPL with
#>>> import pms1w
# OSCI webserver should run
# using same webserver lib and settings.toml file

# to stop if REPL [ctrl][c] or [crtl][d].. not work
# reconnect the PICO_W USB or RESET ( by PB pin30 to GND )

import board
import pwmio
import digitalio
led = digitalio.DigitalInOut(board.LED) # ___________ actually on PICO W LED is driven by wifi chip
led.direction = digitalio.Direction.OUTPUT
led.value = True  # _________________________________ after boot LED ON helps to see its working.. at a timed job ( here getAins() ) blink

# from https://docs.circuitpython.org/en/latest/shared-bindings/analogbufio/index.html try:
import analogbufio
import array

import os
import time

import micropython

import gc # ___ micropython garbage collection ______ to deal with memory problems
# _____________ use gc.mem_free()
# _____________ use gc.collect() ? right in the html svg OSCI tool

# ___________________________________________________ expect file: settings.toml
# ___________________________________________________ login to users router WIFI
WIFI_SSID = os.getenv('WIFI_SSID')
WIFI_PASSWORD = os.getenv('WIFI_PASSWORD')

# ___________________________________________________ MAKE WEB Site on fix IP and using dynamic data
WIFI_IP = os.getenv('WIFI_IP')
WIFI_PORT = os.getenv('WIFI_PORT')
THIS_OS = os.getenv('THIS_OS')
THIS_REVISION = os.getenv('THIS_REVISION')

DIAG = True # False # _______________________________ global print disable switch
DIAG = bool(os.getenv('DIAG'))

def dp(line=" ", ende="\n"):
    if DIAG : print(line, end=ende)


TESTPWM = bool(os.getenv('TESTPWM',0) ) # _____________ optional testpuls output, use a jumperwire GP16 to GP26
if ( TESTPWM ) :
    # TEST OUTPUT PULS on pin21 GP16
    pwm = pwmio.PWMOut(board.GP16, frequency=1000)
    pwm.duty_cycle = 2 ** 15 # 50%
    dp("+++ option test puls: 1kHz at pin 21, GP16 enabled, you can place a jumper to pin 31 GP26 A0")

import socketpool
from ipaddress import ip_address
import wifi

from adafruit_httpserver import Server, Request, Response, POST

#buttontxt = ["fast",">>>>",">>>",">>",">","slow"]
rates=[500000,100000,50000,10000,5000,1000]     # called sample rate to bufio.read()
buttontxt = ["500k","100k"," 50k"," 10k"," _5k"," _1k"]


def HtmlStyle() :
    HtmlStyle =   '<style>'
    HtmlStyle +=   'html {font-family: "Times New Roman", Times, serif; background-color: lightgreen;'
    HtmlStyle +=   'display:inline-block; margin: 0px auto; text-align: center;}'
    HtmlStyle +=   'h1{color: deeppink; width: 200; word-wrap: break-word; padding: 2vh; font-size: 35px;}'
    HtmlStyle +=   'p{font-size: 1.5rem; width: 200; word-wrap: break-word;}'
    HtmlStyle +=   '.button{font-family: {font_family};display: inline-block;'
    HtmlStyle +=   'background-color: black; border: none;'
    HtmlStyle +=   'border-radius: 4px; color: white; padding: 16px 40px;'
    HtmlStyle +=   'text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}'
    HtmlStyle +=   'p.dotted {margin: auto; width: 75%; font-size: 25px; text-align: center;}'
    HtmlStyle +=   '</style>'
    return HtmlStyle

# ___________________________________________________ HTTP SERVER
def html_index_page():
    htmldata =  '<!DOCTYPE html><html><head><title>KLL Pico W Web Server</title>'
    htmldata += HtmlStyle()
    htmldata += '</head><body>'
    htmldata += '<h1>KLL Pico W Web Server : PMS1W OSCI</h1>'
    htmldata += '<h2> from Circuit Python ' + THIS_OS + '</h2>'
    htmldata += '<p> rev: '+THIS_REVISION+'</p>'
    htmldata += '<img src="https://www.raspberrypi.com/documentation/microcontrollers/images/picow-pinout.svg" >'
    htmldata += '<p> image PICO W </p><hr></br><a href="/osci_details" target="_blank" ><b>osci: PMS1W / PoorManScope 1 channel WIFI</b></a> </br><hr>'
    htmldata += '<p>  <a href="http://kll.byethost7.com/kllfusion01/infusions/articles/articles.php?article_id=218" target="_blank" ><b>kll engineering</b></a> </p>'
    htmldata += '</body></html>'
    return htmldata

def button(txt="text") :
    svg =   "<svg width=\"110\" height=\"55\" xmlns=\"http://www.w3.org/2000/svg\">"
    svg +=  "<style>.button2 { fill: #007bbf;  cursor: pointer; }"
    svg +=  ".button2:hover { fill: #006919; }"
    svg +=  "</style>"
    svg +=  "<g onmouseup=\"buttonClick(evt)\">"
    svg +=  "<rect class = \"button2\" x=\"20\" y=\"1\" rx=\"5\" ry=\"5\" width=\"90\" height=\"50\"/>"
    svg +=  f"<text x=\"38\" y=\"35\" font-size=\"1.8em\">{txt}</text>"
    svg +=  "</g>"
    svg +=  "</svg>"
    return svg

def html_speed_buttons():
    # need operation buttons for use different rates
    htmldata = "<div><hr>"
    htmldata += "<table align=\"center\"><tr><td width=\"10%\"></td></tr>\n"
    htmldata += "<tr>"

    for i in range(0, 6, 1) :
        htmldata += "<td width=\"10%\">"
        htmldata += f"<a href=\"/osci{i}\" style=\"display: block;\">"  # ___ html link with svg button
        htmldata += button( buttontxt[i] )
        htmldata += "</a>"
        htmldata += "</td>"

    htmldata += "</tr></table>\n"
    htmldata += "<hr></div>"
    return htmldata

def html_osci_details_page():
    htmldata =  '<!DOCTYPE html><html><head><title>KLL Pico W Web Server OSCI detail</title>'
    htmldata += HtmlStyle()
    htmldata += '</head><body>'
    htmldata += '<div><p> <b> PICO W measure Analog A0 pin31 to array</b> </p> <p> pls select sample rate </p>'
    htmldata += html_speed_buttons()
    htmldata += "</div>"
    htmldata += '</div></body></html>'
    return htmldata

def check_mem() :
    dp("\n_m_ need clean? available heap RAM {:} ".format(  gc.mem_free() ) )
    gc.collect() # __________________________________ hope that cleans up some RAM
    dp("_m_ after clean available heap RAM {:} ".format(  gc.mem_free() ) )

def html_osci_page(rate = 500000):
    #global thisdt, thisHz, thissampletime

    check_mem()
    osci = get_OSCI(rate) # _________________________ returns a array with 320 values of 0 .. 4096 of A0 via bufio
    #check_mem()

    many = len(osci)

    htmldata =   '<!DOCTYPE html><html><head><title>KLL Pico W OSCI (PMS1W)</title>'
    htmldata += HtmlStyle()
    htmldata += '</head><body>'
    htmldata +=  '<div><p> <b> PICO W measure Analog A0 pin31 to array</b> </br></br> no autorefresh / sampling at YOUR manual browser refresh</br>or use below menu to change sample rate</p>'
    htmldata +=  " <hr><p>{:d} samples in {:.2f} millis, </br>that is {:.3f} millis sample time or {:.1f} Hz </br>called: {:d} rate,</p>".format(many, thisdt,thissampletime,thisHz,rate)

    smin = 4096 # from a 12 bit ADC
    smax = 0
    for i in range(0, many,1) :
        osci[i] = int( osci[i]/16 ) # _______________ new range from bufio back to 4096
        if ( osci[i] < smin ) : smin = osci[i]
        if ( osci[i] > smax ) : smax = osci[i]
        #dp(" i: {} osci: {}".format (i,osci[i]))
    htmldata +=  "<p> MIN : {:d}, MAX : {:d} of 4096 aka 3.3 Volt</p>".format(smin,smax)

    htmldata +=  "<table align=\"center\"><tr><td width=\"10%\"></td></tr>\n"
    htmldata +=  "<tr><td>\n"

    # we measure to a array over 320 samples the numbers are Volt at the A0 pin ( 0 .. 3.3 ) as 4096.
    #our plot area is 640 * 640 in a 660 * 670 svg rectangle, here try the math for it
    scopew = 640
    svgw = scopew+20
    scopeh = 640
    svgh = scopeh+30
    htmldata += "<svg width=\"{:d}\" height=\"{:d}\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">".format(svgw, svgh)
    htmldata += "<desc>show trend line {:d} data samples </desc>".format(many)
    htmldata += ("<!-- grey box with 3D style blue border-->")
    htmldata += "<rect x=\"1\" y=\"1\" width=\"{:d}\" height=\"{:d}\"  stroke=\"#000088\" stroke-width=\"1\" />".format(svgw, svgh)
    htmldata += "<rect x=\"2\" y=\"2\" width=\"{:d}\" height=\"{:d}\"  stroke=\"#0000aa\" stroke-width=\"1\" />".format((svgw-4),(svgh-4))
    htmldata += "<rect x=\"3\" y=\"3\" width=\"{:d}\" height=\"{:d}\"  fill=\"#FFF8DC\" stroke=\"#0000ff\" stroke-width=\"1\" />".format((svgw-6),(svgh-6))
    htmldata += ("<g id=\"trend\" style=\"visibility:visible; stroke:red; stroke-width:2; \" > ")

    xo = 10
    x1 = 0
    x2 = 0
    yo = 15
    y1 = 0
    y2 = 0 #_ offset from  top left
    yf1=0.0
    yf2=0.0

    for xi in range(0, many-1,1) :
        yf1 = osci[xi]
        yf2 = osci[xi + 1]
        x1 = xo + xi*2
        x2 = x1 + 2
        y1 = yo + scopeh - int(yf1 *scopeh/4096)
        y2 = yo + scopeh - int(yf2 *scopeh/4096)
        htmldata += "<line x1=\"{:d}\" y1=\"{:d}\" x2=\"{:d}\" y2=\"{:d}\" />\n".format(x1, y1, x2, y2)

    htmldata += ("</g>")
    htmldata += ("</svg>\n")

    htmldata +=  "</td><td width=\"10%\">&nbsp;&nbsp;<br></td><td width=\"10%\"></table></br>\n"
    htmldata +=  '</div>'

    #htmldata +=  '<hr></br><a href="/osci_details" ><b>osci-speed-details</b></a> </br></br> <hr>'
    htmldata += html_speed_buttons()
    htmldata +=  '</body></html>'
    return htmldata

def setup_webserver() :
    global server
    dp("\n___ PICO W: Hello World! start webserver (STA) for OSCI PMS1W\n")
    dp("___ Connecting to router {:s} OR CHECK THE 'settings.toml' FILE".format( WIFI_SSID) )
    wifi.radio.set_ipv4_address( # __________________ fixIP ( requires > CP 8.0.0 beta 4 )
        ipv4=ip_address(WIFI_IP),
        netmask=ip_address("255.0.0.0"),
        gateway=ip_address("192.168.1.1"),
        ipv4_dns=ip_address("192.168.1.1"),
    )
    wifi.radio.connect(WIFI_SSID, WIFI_PASSWORD)
    dp("___ Connected to {:s}".format( WIFI_SSID) )
    dp("___ Listening on http://{:s}:{:d} ".format(str(wifi.radio.ipv4_address),WIFI_PORT ) )

    pool = socketpool.SocketPool(wifi.radio)

    server = Server(socket_source=pool,debug=True)  # ____________________ make a WEB SERVER

    @server.route("/")
    def base(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic index.html")
        return Response(request, html_index_page(), content_type='text/html' )

    # _______________________________________________ to adjust the wait loops in the data sampling to get a required sampling time for your buttons

    @server.route("/osci0") #  [FAST]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[0]), content_type='text/html' )

    @server.route("/osci1") # [>>>>]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[1]), content_type='text/html' )

    @server.route("/osci2") # [>>>]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[2]), content_type='text/html' )

    @server.route("/osci3") # [>>]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[3]), content_type='text/html' )

    @server.route("/osci4") # [>]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[4]), content_type='text/html' )

    @server.route("/osci5") # [slow]
    def osci(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci.html")
        return Response(request, html_osci_page(rates[5]), content_type='text/html' )


    @server.route("/osci_details")
    def osci_details(request):  # pylint: disable=unused-argument
        dp("\nwifi served dynamic osci_details.html")
        return Response(request, html_osci_details_page(), content_type='text/html' )

    server.start(host=str(wifi.radio.ipv4_address),port=WIFI_PORT) # ____ startup the server

def run_webserver() :
    #global server
    try:
        server.poll()
    except OSError:
        print("ERROR server poll")

    time.sleep(0.5) # now can stop with [ctrl][c] again OR NOT ( USE USB RECONNECT )

# ___________________________________________________ MAKE A OSCI DATA Array PICO W IO A0 pin 31
arrylong = 320
thisdt = 0.0
thissampletime = 0.0
thisHz = 0.0


def get_OSCI(rate=500000) :
    global arrylong, thisdt, thisHz, thissampletime
    led.value = False  # _________________________________ BLINK
    dp("___ read PICO W A0 (pin31) to array")
    osciH = array.array("H", [0x0000] * arrylong )
    #adcbuf = analogbufio.BufferedIn(board.GP26, osciH, rate) # _____________________________________ ERROR TypeError: extra positional arguments given
    #adcbuf = analogbufio.BufferedIn(board.GP26, osciH, sample_rate=rate) # _________________________ thanks 'anecdata' CP800b5 again TypeError: extra positional arguments given
    adcbuf = analogbufio.BufferedIn(board.GP26, sample_rate=rate)
    startt = time.monotonic_ns()
    #adcbuf.read()
    adcbuf.readinto(osciH) # ________________________ move buffer to here
    stopt = time.monotonic_ns()
    adcbuf.deinit()
    dtns = ( stopt - startt ) * 1.0 # _______________ nano sec to float
    thisdt = dtns / 1000000.0 # _____________________ milli sec float
    thissampletime = thisdt / ( arrylong*1.0 ) # also millis
    thisHz = 0
    if ( thissampletime > 0 ) : # ___________________ no idea why that sometimes goes wrong
        thisHz = 1000.0/(thissampletime*1.0)
    else :
        dp("time calc failed")
    dp("osci dt {:.1f} ns, {:d} samples in {:.2f} millis, that is {:.3f} millis sample time or {:.1f} Hz, called rate {:d} Hz".format(dtns,arrylong,thisdt,thissampletime,thisHz,rate))
    led.value = True  # _____________________________ BLINK

    return osciH # array of samples, range 0 .. 4095

# ___________________________________________________ MAIN

setup_webserver()

while True :
    run_webserver()
