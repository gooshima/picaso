$(document).ready(function(){
    $( "#dialog-picasoPhoto" ).dialog({
        zIndex: 4001,
        autoOpen:false,
        modal: true,
        width: 'auto',
        height: 'auto',
        height: window.innerHeight * 0.85,
        maxWidth: window.innerWidth * 0.8
        //show: 'clip'

        //maxHeight: window.innerHeight * 0.8
    });

    $( "#dialog-picasoMovie" ).dialog({
        zIndex: 4001,
        autoOpen:false,
        modal: true,
        width: 'auto',
        height: 800,
        maxWidth: window.innerWidth * 0.8,
        maxHeight: 800,
        //show: 'clip'
    });

    $( "#dialog-picasoController" ).dialog({
        zIndex: 9999,
        autoOpen:false,
        modal: false,
        width: 'auto',
        maxWidth: window.innerWidth * 0.8,
        maxHeight: 300,
        //show: 'clip'
    });


});

$(function(){



    $('#btn-watchVideo').click(function(){
        $( "#dialog" ).dialog({
            modal: true
        });
    });

    $('#btn-picasoPhoto').click(function(){
        $('#iframe-picasoPhoto').attr('src', '../index.html');
        $('#dialog-picasoPhoto').dialog('open');
    });

    $('#btn-picasoMovie').click(function(){
        $('#dialog-picasoMovie').dialog('open');
    });

    $('#btn-picasoController').click(function(){
        $('#iframe-picasoContoroler').attr('src', '../pi.html');
        $('#dialog-picasoController').dialog('open');
    });








});