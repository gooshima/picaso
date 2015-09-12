/*
 * クリックして色を取得する座標を固定, もう一度クリックして解除
 */
var IMG_URL = [
        //'http://localhost:3000/img/picaso_mini.jpg',
        //'http://localhost:3000/img/picaso_mini.jpg'
        'img/picaso_mini.jpg',
        'img/picaso_mini.jpg'
    ],
    canvas,
    ctx,
    colors = [],
    img,
    display,
    inputs,
    picker,
    holding = false;

function $id(id) { return document.getElementById(id); }

function init() {
    // 画像
    img = $id('img');
    img.addEventListener('load', loadComplete, false);
    img.src = IMG_URL[Math.floor(Math.random () * IMG_URL.length)];

    // 色情報を取得するための canvas
    canvas = document.createElement('canvas');
    ctx = canvas.getContext('2d');

    // カラー表示
    display = $id('display');
    inputs = {
        r: $id('r'), g: $id('g'), b: $id('b'), // RGB
        hex: $id('hex'), // HEX
        h: $id('h'), s: $id('s'), l: $id('l') // HSL
    };
    // ピッカー
    picker = $id('cursor');
}

function loadComplete() {

    canvas.width = img.width;
    canvas.height = img.height;
    ctx.drawImage(img, 0, 0);

    var w, h, data, n, r, g, b, hsl, hue, s, l;

    // 画像の色情報を取得
    w = canvas.width;
    h = canvas.height;
    data = ctx.getImageData(0, 0, w, h).data;


    var rList = [];
    var totalList = [];
    for (var y = 0; y < h - 1; y++) {
        for (var x = 0; x < w - 1; x++) {
            //if(x % 50 === 0){
            n = x * 4 + y * w * 4;
            // RGB
            //console.log("r is " + r);
            r = data[n];
            g = data[n + 1];
            b = data[n + 2];
            // HSL
            hsl = rgbToHsl(r, g, b);
            hue = Math.round(hsl[0]);
            s = Math.round(hsl[1] * 100);
            l = Math.round(hsl[2] * 100);
            colors.push([r, g, b, hue, s, l]);

            if (x % 50 === 0) {
                rList.push([r, g, b]);
            }

                //$('#rgbAverage').append('<div class="rgbDisplay" id="rgb-' + y + '"></div>'+ r + '-' + g + '-' + b);
                //$id('rgb-' + y).style.backgroundColor = 'rgb(' + r + ', ' + g + ', ' + b + ')';
            //}


        }
        //$('#rgbAverage').append('<hr>');
        //$('#rgbAverage').append('<div class="rgbDisplay" id="rgb-' + y + '"></div>'+ r + '-' + g + '-' + b);
        //$id('rgb-' + y).style.backgroundColor = 'rgb(' + r + ', ' + g + ', ' + b + ')';

        //console.log(r + " | " + g + " | " + b);
    }

    console.log(rList.length + '個のRGBを検出しました。');
    //return;

    //重複を、重複数を記録し、別の配列にする。
    var result = [];
    var target;
    $.each(rList, function (index, val) {
        //alert(index + "回めのループ");
        if (val != undefined) {
            var sameNum = 0;
            target = target = val.toString();

            $.each(rList, function (i, v) {
                //result = [sameNum + '回 ~ ' + v];
                if (v != undefined) {
                    if (v.toString() === target) {
                        //console.log(v);
                        sameNum++;
                        result = [];
                        result = [sameNum + "回出現 ~ ", v];
                        //alert(i + "番目は = " + v + "!!!");
                        //rList.splice(i, 1);
                    }
                }else{
                    //result.push([sameNum, val]);
                }
            });
            totalList.push(result);
            sameNum = 0;
        }
    });

    //alert("色の割合を表示します。");
    arr = unique(totalList);
    $.each(arr, function (index, val) {
        if (val != undefined) {
            $('#rgbAverage').append('<div>'+val+'</div>');
        }
    });


    img.addEventListener('mousemove', mouseMove, false);
    img.addEventListener('mousedown', mouseDown, false);
}

function mouseMove(e) {
    if (holding) return;

    var mx, my, n, color, r, g, b, h, s, l;

    // カーソル位置からピクセルの配列位置を取得
    n = e.offsetX + e.offsetY * (img.width - 1);
    // Firefox の場合 (offsetX, offsetY から値が取れない場合)
    if (!n && n !== 0) {
        var pos = getElementPosition(img);
        n = (e.layerX - pos.x) + (e.layerY - pos.y) * (img.width - 1);
    }

    color = colors[n];
    if (!color) return;
    r = color[0]; g = color[1]; b = color[2];
    h = color[3]; s = color[4]; l = color[5];

    // カラー表示
    display.style.backgroundColor = 'rgb(' + r + ', ' + g + ', ' + b + ')';

    // RGB 情報
    inputs.r.value = r; inputs.g.value = g; inputs.b.value = b;
    inputs.hex.value = (b | (g << 8) | (r << 16)).toString(16).toUpperCase();

    // HSL 情報
    inputs.h.value = h; inputs.s.value = s; inputs.l.value = l;
}

/*
 * Firefox で要素上のマウス座標を取得するための要素位置を取得する関数
 */
function getElementPosition(elem) {
    var html, body, scrollLeft, scrollTop, rect;

    html = document.documentElement;
    body = document.body;
    scrollLeft = body.scrollLeft || html.scrollLeft;
    scrollTop = body.scrollTop || html.scrollTop;
    rect = elem.getBoundingClientRect();

    // Firefox では　getBoundingClientRect　で取得した値が 1px 少なくなるので足しておく
    return {
        x: Math.round(rect.left + 1 - html.clientLeft + scrollLeft),
        y: Math.round(rect.top + 1 - html.clientTop + scrollTop)
    };
}

function mouseDown(e) {
    alert("マイコンにシリアルでRGB送信");
    /*
    if (!holding) {
        picker.style.display = 'block';
        picker.style.left = e.clientX - 2 + 'px';
        picker.style.top = e.clientY - 2 + 'px';
    } else {
        picker.style.display = 'none';
    }
    holding = !holding;
    */
}

/**
 * RGB -> HSL
 * それぞれ 0 ~ 255 で指定
 */
function rgbToHsl(r, g, b) {
    var h, s, l,
        max = Math.max(Math.max(r, g), b),
        min = Math.min(Math.min(r, g), b);

    // Hue, 0 ~ 359
    if (max === min) {
        h = 0;
    } else if (r === max) {
        h = ((g - b) / (max - min) * 60 + 360) % 360;
    } else if (g === max) {
        h = (b - r) / (max - min) * 60 + 120;
    } else if (b === max) {
        h = (r - g) / (max - min) * 60 + 240;
    }
    // Saturation, 0 ~ 1
    s = (max - min) / max;
    // Lightness, 0 ~ 1
    l = (r *  0.3 + g * 0.59 + b * 0.11) / 255;

    return [h, s, l, 'hsl'];
}

window.onload = function() {
    init();
};



//TOOD scan カスタマイズ
function scanImg(){
    var element = document.getElementById('img'); // hoge というIDがついたやつを取得する
    var rect = element.getBoundingClientRect();
    console.log('left is ' + rect.left);   // x座標(絶対座標)
    console.log("top is " + rect.top);    // y座標(絶対座標)
    console.log("width is " + rect.width);  // 幅
    console.log("height is " + rect.height); // 高さ

    //画像の左上端を0とする
    var e = new jQuery.Event("mousedown");
    e.offsetX = 50;
    e.offsetY = 100;
    mouseMoveYeah(e.offsetX, e.offsetY);
    mouseDownYeah(rect.left + e.offsetX, rect.top + e.offsetY); //マウスのポイントを表示
    console.log("選択範囲 X " + e.offsetX);  // 幅
    console.log("選択範囲 Y " + e.offsetY); // 高さ

}

function mouseDownYeah(hey1, hey2){
    /*
    if (!holding) {
        picker.style.display = 'block';
        picker.style.left = hey1 - 2 + 'px';
        picker.style.top = hey2 - 2 + 'px';
    } else {
        picker.style.display = 'none';
    }
    holding = !holding;
    */
}
function mouseMoveYeah(hey1, hey2) {
    if (holding) return;

    var mx, my, n, color, r, g, b, h, s, l;

    // カーソル位置からピクセルの配列位置を取得
    n = hey1 + hey2 * (img.width - 1);
    // Firefox の場合 (offsetX, offsetY から値が取れない場合)
    if (!n && n !== 0) {
        var pos = getElementPosition(img);
        n = (hey1 - pos.x) + (hey2 - pos.y) * (img.width - 1);
    }

    color = colors[n];
    if (!color) return;
    r = color[0]; g = color[1]; b = color[2];
    h = color[3]; s = color[4]; l = color[5];

    // カラー表示
    display.style.backgroundColor = 'rgb(' + r + ', ' + g + ', ' + b + ')';

    // RGB 情報
    inputs.r.value = r; inputs.g.value = g; inputs.b.value = b;
    inputs.hex.value = (b | (g << 8) | (r << 16)).toString(16).toUpperCase();

    // HSL 情報
    inputs.h.value = h; inputs.s.value = s; inputs.l.value = l;

    showRGB(r,g,b);
}

function showRGB(r, g, b){
    $('#rgbAverage').append('<div class="rgbDisplay"></div>'+ r + '-' + g + '-' + b);
    //$('#rgbAverage').append('<li>' + r + ' ' + g + ' ' + b + ' ' + '</li>');
}

$(document).ready(function(){
    setTimeout(function(){
        scanImg();
    }, 100);
});




//TODO temporary.....
function unique(array) {
    var storage = {};
    var uniqueArray = [];
    var i, value;
    for (i = 0; i < array.length; i++) {
        value = array[i];
        if (!(value in storage)) {
            storage[value] = true;
            uniqueArray.push(value);
        }
    }
    return uniqueArray;
}