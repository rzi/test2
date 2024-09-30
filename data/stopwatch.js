function btnOn(){
    console.log("click start");
    const status = 1;
    var startValue = document.getElementById("setON").value;
    var stoptValue = document.getElementById("setOFF").value;
    console.log("Start value = " + startValue +" stop value = "+stoptValue);
    const url = new URL('http://192.168.100.7/manual2');
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
            }else{
                myspan.innerText = "nieaktywny";
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
    const url = new URL('http://192.168.100.7/manual2');
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
            }else{
                myspan.innerText = "nieaktywny";
            }
        });
    }else{
        alert("ustawe czs włączenia i wyłączenia !")
    }
}

function zegar() {
  var state = document.getElementById("state");
  if (state.innerText == "Stan OFF"){
    state.style.color = 'white'
    state.style.backgroundColor = 'green'
  }else{
    state.style.color = 'white'
    state.style.backgroundColor = 'red'
  }
  setTimeout("zegar()", 1000);
}
zegar();
const hamburger = document.querySelector('.hamburger');
const nav = document.querySelector('.navigation');
const handleClick = () => {
  hamburger.classList.toggle('hamburger--active');
  hamburger.setAttribute('aria-expanded', hamburger.classList.contains('hamburger--active'));
  nav.classList.toggle('navigation--active');
}
hamburger.addEventListener('click', handleClick);
