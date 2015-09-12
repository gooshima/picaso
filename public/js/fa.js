var socket = io();

var isFlashAir = false;

var urlBase = 'http://localhost:3000'; //TODO debug
//var urlBase = 'http://neonneon1';

var appUrl = urlBase + '/app';
var cgiUrl = urlBase + "/command.cgi?op=131&ADDR=0&LEN=3&DATA=";

/**
 * FlashAirに対してHTTP通信 GETをする
 * @param param
 */
function flashair_get(param) {
    var request = new XMLHttpRequest();
    request.open("GET", param, false);
    request.send(null);
    //通信結果
    //console.log(request.responseText);
}

/**
 * 共有メモリを初期化
 */
function memory_init() {
    socket.emit('sendmsg', '0');
    //var url = cgiUrl + 0;
    //flashair_get(url);
}

/**
 * 共有メモリに書き込み
 * @param argByte
 */
function write_memory(argByte) {
    var url = cgiUrl + argByte;
    flashair_get(url);
}

$(document).ready(function () {

    //シリアル通信用処理
    //var socket = io();
    $('.ioBtn').click(function(){
        var val = $(this).val();
        console.log("btn " + val + " clicked...");
        socket.emit('sendmsg', val.toString());
    });
    $('#ioTextBtn').click(function () {
        //socket.emit('sendmsg','2');
        console.log("ioTextBtn clicked....");
        socket.emit('sendmsg', $("#patternNum").val());
    });
    socket.on('recvmsg', function (data) {
        console.log("socket is....");
        console.log(data);
        //$('h1').text(data);
    });


    socket.emit('sendmsg', '0');
    $('#iconStop').hide();
    $('#neonDisp').hide();
    var isPlay = false;

    var audio = document.getElementById("demoBgm");


    //$('.stopBtn').click(function(){
    //    audio.stop();
    //});
    //$('#syncStartBtn').click(function () {
    $('.playBtn').click(function (){
        //$('.playBtn').hide();
        //$('.stopBtn').show();
        audio.load();

        if (isPlay) {
            //音楽停止
            //ゼロを書き込み
            //write_memory(0);
            audio.pause();
            //$('.playBtn').show();
            //$('.stopBtn').hide();
            $('#syncTxt').text('同期 開始');
            isPlay = false;
            //write_memory(0);
            socket.emit('sendmsg', '0');
            window.location.href = "index.html";
        }
        else {
            //TODO ちょっとずらして音を鳴らす
            $("#demoBgm").trigger('play');
            console.log('audio start');

            var tid = setTimeout(checkAudioTime, 100);
            function checkAudioTime() {
                playTime = audio.currentTime;
                console.log('playtime is');
                console.log(playTime);
                tid = setTimeout(checkAudioTime, 100); // repeat myself
                if(playTime > 0){
                    abortTimer();
                    console.log("==== audio started!! ===");
                }
            }
            function abortTimer() { // to be called when you want to stop the timer

                clearTimeout(tid);

                //音源を開始
                $('#neonDisp').show();
                $('#syncTxt').text('同期 Stop');
                $('#iconStart').hide();
                $('#iconStop').show();
                isPlay = true;

                //音楽再生バイト
                socket.emit('sendmsg', '9');

            }
        }
    });
});