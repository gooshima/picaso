var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.get('/',function(req,res){
    res.sendfile('index.html');
});

//app.use(express.static(__dirname + '/public'));
app.use(express.static(__dirname + '/'));

http.listen(3000,function(){
    console.log('listen 3000 port');
});

//Create the AlchemyAPI object
//var AlchemyAPI = require('./alchemyapi');
var AlchemyAPI = require('./server/alchemyapi');
var alchemyapi = new AlchemyAPI();

//var PicasoApp = require('./server/picasoapp');
var PicasoApp = require('./server/picasoapp');
var picasoapp = new PicasoApp();


picasoapp.testFunc();

var hoge = function(){
    alert("a");
};