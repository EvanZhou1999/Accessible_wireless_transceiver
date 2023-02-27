// PROGMEM: Store data in flash (program) memory instead of SRAM, 

const char INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
    <head>
        <Title>Accessible Wireless Switch</Title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>
            html {
                font-family: Arial, Helvetica, sans-serif;
                text-align: center;
            }
            h1 {
              font-size: 1.8rem;
              color: white;
            }
            h2{
              font-size: 1.5rem;
              font-weight: bold;
              color: #DE8344;
            }
            .topnav {
                overflow: hidden;
                background-color: #333333;
            }
            body {
                margin: 0;
            }
            .content {
                padding: 30px;
                max-width: 600px;
                margin: 0 auto;
            }
            .card {
                background-color: #F8F7F9;;
                box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5);
                padding-top: 10px;
                padding-bottom: 20px;
                border-radius: 30px;
            }
            .button {
                padding: 15px 50px;
                font-size: 24px;
                text-align: center;
                outline: none;
                color: #fff;
                background-color: #DE8344;
                border: none;
                border-radius: 15px;
                -webkit-touch-callout: none;
                -webkit-user-select: none;
                -khtml-user-select: none;
                -moz-user-select: none;
                -ms-user-select: none;
                user-select: none;
                -webkit-tap-highlight-color: rgba(0,0,0,0);
            }
            .button:active {
                background-color: #DEA27E;
                box-shadow: 2 2px #CDCDCD;
                transform: translateY(2px);
            }
            .state {
                font-size: 1.5rem;
                color: #8c8c8c;
                font-weight: bold
            }
        </style>
    </head>
        
    <body>
        <!-- Div for the top title bar -->
        <div class="topnav">
            <h1>Accessible Wireless Switch<h1>
        </div>
        <div style="background: #DE9558; height: 10px;"></div>
        <!-- Div for the body of the page -->
        <div class="content">
            <div class="card">
                <p class="state">
                    This page is for settings and configurations.
                </p>
            </div>
            <br>
            <br>
            <div class="card">
                <h2>Switch Control</h2>
                <p class="state">Click here to activate switch once:</p>
                
                <p>
                    <button id="button_ACT" class="button">Activate Switch</button>
                </p>

            </div>
            <br>
            <br>
            <div class="card">
                <h2>Device Documentations</h2>
                <p class="state">Get help here</p>
                
                
                <button id="button_help" class="button" onclick="window.location.href='https://drive.google.com/drive/folders/1nD83zP2qYV-7mvF2n2yecVb5Hgfm7Zpj?usp=share_link';">
                    User Manual
                </button>
                <br>
                <br>
                <button id="button_help" class="button" onclick="window.location.href='https://github.com/EvanZhou1999/Accessible_wireless_transceiver';">
                    Source Code
                </button>
                

            </div>
            
        </div>
        
        <script>
            document.getElementById('button_ACT').addEventListener('click', switchClick);
            
            function switchClick() {
                // Send request
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {};
                const path = "control/click";
                xhttp.open("GET", path, true);
                xhttp.send();
              }

        </script>
    </body>
</html>


)=====";
