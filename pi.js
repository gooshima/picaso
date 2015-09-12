var device_name = '/dev/ttyACM0';//TODO ACM1になったり。。。
//var device_name = '/dev/ttyACM1';//TODO ACM1になったり。。。

var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require('serialport').SerialPort;
var serial = new SerialPort(device_name, {
    //baudrate:57600
    baudrate:115200
});

app.get('/',function(req,res){
    res.sendfile('index.html');
});

app.use(express.static(__dirname + '/'));

serial.on('open',function(){
    console.log('open');
});

serial.on('data',function(data){
    io.emit('recvmsg',data.toString());
});

io.on('connection',function(socket){
    socket.on('sendmsg',function(msg){
        console.log("here....");
        serial.write(msg,function(err,results){
            console.log(msg);
        });
    });
});

http.listen(3000,function(){
    console.log('listen 3000 port');
});