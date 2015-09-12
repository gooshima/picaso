var http = require('http');
var fs = require('fs');

exports = module.exports = PicasoApp;

function PicasoApp() {

    this.testFunc = function(){
        console.log("testFunc called...");
    }

}

function hoge(){
    alert("aaa");
}

(function (exports) {
    exports.hello = function(){return 'Hello world';};
})(typeof exports === 'undefined' ? this.mymodule = {} : exports);
