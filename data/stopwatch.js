function btnOn(){
    console.log("click start");
    const status = 1;
    var startValue = document.getElementById("setON").value;
    var stoptValue = document.getElementById("setOFF").value;
    console.log("Start value = " + startValue +" stop value = "+stoptValue);
    const url = new URL('http://192.168.100.7/stopwatch2');
    url.searchParams.append('start', startValue);
    url.searchParams.append('stop', stoptValue);
    url.searchParams.append('status', status);
    console.log("url " + url.search);
    if (startValue !="" && stoptValue !="" ){
        fetch(url)
        .then((response) => response.json())
        .then((text) => {
            let status = parseInt(text);
            console.log("status "  +status);
            var myspan = document.getElementById('status');
            if (status == 1){
                myspan.innerText = "aktywny";
                myspan.color = "red";
            }else{
                myspan.innerText = "nieaktywny";
                myspan.color = "green";
            }
        });
    }else{
        alert("ustawe czs włączenia i wyłączenia !")
    }
}
function btnOff(){
    console.log("click stop");
    const status = 0;
    var startValue = document.getElementById("setON").value;
    var stoptValue = document.getElementById("setOFF").value;
    console.log("Start value = " + startValue +" stop value = "+stoptValue);
    const url = new URL('http://192.168.100.7/stopwatch2');
    url.searchParams.append('start', startValue);
    url.searchParams.append('stop', stoptValue);
    url.searchParams.append('status', status);
    console.log("url " + url.search);
    if (startValue !="" && stoptValue !="" ){
        fetch(url)
        .then((response) => response.json())
        .then((text) => {
            let status = parseInt(text);
            console.log("status "  +status);
            var myspan = document.getElementById('status');
            if (status == 1){
                myspan.innerText = "aktywny";
                myspan.color = "red";
            }else{
                myspan.innerText = "nieaktywny";
                myspan.color = "green";
            }
        });
    }else{
        alert("ustawe czs włączenia i wyłączenia !")
    }
}
const hamburger = document.querySelector('.hamburger');
const nav = document.querySelector('.navigation');
const handleClick = () => {
  hamburger.classList.toggle('hamburger--active');
  hamburger.setAttribute('aria-expanded', hamburger.classList.contains('hamburger--active'));
  nav.classList.toggle('navigation--active');
}
hamburger.addEventListener('click', handleClick);
var checkbox = document.querySelector("input[name=input1]");

checkbox.addEventListener('change', function() {
    var myCheckbox;
    var startValue = document.getElementById("setON").value;
    var stoptValue = document.getElementById("setOFF").value;
  if (this.checked) {
    console.log("Checkbox is checked..");
    myCheckbox =1;
  } else {
    console.log("Checkbox is not checked..");
    myCheckbox=0;
  }
  const url = new URL('http://192.168.100.7/stopwatch2');
    url.searchParams.append('mycheckbox', myCheckbox);
    url.searchParams.append('start', startValue);
    url.searchParams.append('stop', stoptValue);
    console.log("url " + url.search);
        fetch(url)
        .then((response) => response.json())
        .then((text) => {
            let status = parseInt(text);
            console.log("status "  +status);
            var myspan = document.getElementById('status');
            if (status == 1){
                myspan.innerText = "aktywny";
                myspan.color = "red";
            }else{
                myspan.innerText = "nieaktywny";
                myspan.color = "green";
            }
        });

});
