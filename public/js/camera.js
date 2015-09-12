//var hey = PicasoApp();

$(document).ready(function(){

    //console.log(PicasoApp);
    //console.log(picasoapp);

    //APIを格納
    navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;
    //window.URLのAPIをすべてwindow.URLに統一
    window.URL = window.URL || window.webkitURL;

    if (!navigator.getUserMedia) {
        alert("カメラ未対応のブラウザです。");
    }

    // 変数
    var canvas = document.getElementById("canvas"),
        context = canvas.getContext("2d"),
        video = document.getElementById("video"),
        btnStart = document.getElementById("start"),
        btnStop = document.getElementById("stop"),
        btnPhoto = document.getElementById("photo"),
        videoObj = {
            video: true,
            audio: false
        };
    btnStart.addEventListener("click", function() {
        var localMediaStream;

        if (navigator.getUserMedia) {

            $('#mode-img-disp').hide();

            navigator.getUserMedia(videoObj, function(stream) {
                localMediaStream = stream;
                video.src = window.URL.createObjectURL(localMediaStream);

            }, function(error) {
                alert("カメラの設定を確認してください。");
                console.error("getUserMedia error: ", error.code);
            });

            btnStop.addEventListener("click", function() {
                localMediaStream.stop();
                //location.reload();
            });

            btnPhoto.addEventListener("click", function() {
                context.drawImage(video, 0, 0, canvas.width, canvas.height);
                $('#picason-msg-disp').show();
                var cvs = canvas;
                var png = cvs.toDataURL('image/png');
                var formData = new FormData();
                formData.append('file', png);

                /*
                $.ajax({
                    type: "POST",
                    url: "/rest/capture/capture",
                    data: formData
                }).done(function( msg ) {

                    console.log("Data Saved: " + msg);
                });
                */

                document.getElementById("newImg").src = png;
                var targetImgSrc = $('#newImgSrc').src;

                var pngDataToAlchemy = png.replace(/^data:image\/(png|jpg);base64,/, "");
                sendImgToAlchemy(pngDataToAlchemy);

                console.log(pngDataToAlchemy);//TODO　画像認識APIに送信するデータ

                //順番表示
                startPicasonText();
            });
        }
    });

    //順番表示
    function startPicasonText() {
        var targetObj = $('.split');
        var delaySpeed = 150;
        var fadeSpeed = 100;
        var targetTxt = targetObj.html();

        targetObj.css({visibility: 'visible'}).children().addBack().contents().each(function () {
            var elmThis = $(this);
            if (this.nodeType == 3) {
                var $this = $(this);
                $this.replaceWith($this.text().replace(/(\S)/g, '<span class="textSplitLoad">$&</span>'));
            }
        });
        var splitLength = $('.textSplitLoad').length;
        targetObj.find('.textSplitLoad').each(function (i) {
            var splitThis = $(this);

            splitThis.delay(i * (delaySpeed)).css({
                display: 'inline-block',
                opacity: '0'
            }).animate({opacity: '1'}, fadeSpeed);
        });

        setTimeout(function () {
            targetObj.html(targetTxt);
        }, splitLength * delaySpeed + fadeSpeed);
    }


    //TODO API
    function sendImgToAlchemy_bk(argContext) {
        alert("Alchemy APIにデータを送信");
        return;
        $.ajax({
            type: "POST",
            url: "http://access.alchemyapi.com/calls/image/ImageGetRankedImageKeywords",
            //'?apikey=5e8b456ecf83ff1ecbaf087c2345fc6698a88765',
            //context: document.body,
            contentType: "application/json",
            data: {
                apikey: "5e8b456ecf83ff1ecbaf087c2345fc6698a88765",
            },
            success: function (data) {
                alert('success');
                console.log(data);
            },
            error: function (e) {
                alert('failure');
                console.log(e);
            }
        });
    }

    /**
     * POST : create
     * 登録処理
     * @param {} req リクエスト
     * @param {} res レスポンス
     * @param {} next 終了メソッド
     */
    function sendImgToAlchemy(req, res, next) {
        return;
        var fs = require("fs");
        var AlchemyAPI = require(Tsunaruts.current + "/alchemyapi");
        var alchemyapi = new AlchemyAPI();
        var form = new Tsunaruts.multipartForm();

        //TODO API使用数注意
        form.parse(req, function (err, fields, files) {
            var imgData = fields.file[0];
            var base64Data = imgData.replace(/^data:image\/png;base64,/, '');
            var filePath = Tsunaruts.current + "/picaso.png";

            fs.writeFile(filePath, base64Data, 'base64', function (err) {

                //Alchemy API から画像タグを取得
                /*
                 var apiOption = {forceShowAll: 0};//0=1個のタグ, 1=複数のタグ
                 alchemyapi.image_keywords('image', filePath, apiOption, function (response) {
                 //console.log(response);
                 res.send(response.imageKeywords);
                 });
                 */
            });
        });
    }


});