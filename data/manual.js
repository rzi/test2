function on() {
    fetch ("http://192.168.100.7/led2on")
    .then(response => {
        response.text();
        window.location.reload();
    })
}
function off() {
    fetch ("http://192.168.100.7/led2off")
    .then(response => {
        response.text();
        window.location.reload();
    })
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
