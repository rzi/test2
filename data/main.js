console.log("start");
var pickrDate;
document.getElementById("calendar").value =pickrDate;
var year, month, day ,hour, minute, seconds
function on() {
  fetch ("http://192.168.100.7/led2on")
}
function off() {
  fetch ("http://192.168.100.7/led2off")
}
function setStart() {
  var startValue = document.getElementById("calendar").value;
  console.log("Start value = " + startValue );
  const url = new URL('http://192.168.100.7/');
  url.searchParams.set('start', startValue);
  if (startValue !=""){
    fetch(url).then(location.reload());
  }else{
    alert("Wybierz datę !")
  }
}
function setStop() {
  var stopValue = document.getElementById("calendar").value;
  console.log("Stop value = " + stopValue );
  const url = new URL('http://192.168.100.7/');
  url.searchParams.set('stop', stopValue);
  if (stopValue !=""){
    fetch(url).then(location.reload());
  }else{
    alert("Wybierz datę !")
  }
}
function zegar() {
  var data = new Date();
  year = data.getFullYear();
  month = data.getMonth()+1;
  day = data.getDate();
  hour = data.getHours();
  minute = data.getMinutes();
  second = data.getSeconds();
  var terazjest = ""+ year + ((month<10)?"/0":"/")+ month + ((day<10)?"/0":"/")+ day + "  " +
   +hour+
  ((minute<10)?":0":":")+minute+
  ((second<10)?":0":":")+second;
  document.getElementById("f1").czas.value = terazjest;
  setTimeout("zegar()", 1000);
}
zegar();
flatpickr("#calendar", {
  enableTime: true,
  defaultDate: new Date(),
  dateFormat: "Y-m-d H:i",
  minuteIncrement: 1,
  onChange: function(selectedDates, dateStr, instance) {
      console.log(`Wybrano datę: ${dateStr}`);
      flatpickr =dateStr;
  }
}); // flatpickr
var items1  = new Array();
var item ="";
function currentloginid() {
  return fetch('http://192.168.100.7/eeprom')
    .then(response => response.json())
}
const storedPromise = currentloginid();
storedPromise.then(storedPromise => {
    for (const key in storedPromise) {
      item="";
      let object = storedPromise[key];
      for (const key2 in object) {
        if (key2 == "type" && object[key2]==1){
          item = "start ";
        }else if(key2 == "type" && object[key2]==0){
          item = "stop ";
        } else if(key2 == "type" && object[key2]==-1){
          return
        }
        if (key2 == "year"){
          item = item + object[key2]
        }
        if (key2 =="month") {
          item = item + "/"+((object[key2] <10) ? "0"+object[key2]: object[key2]);
        }
        if (key2 =="day") {
          item = item + "/"+((object[key2] <10) ? "0" + object[key2]: object[key2]) + " godzina ";
        }
        if (key2 =="hour") {
          item = item + object[key2] +":";
        }
        if (key2 =="minute") {
          if (object[key2]<10){
            item = item +((object[key2] <10) ? "0"+object[key2]: object[key2]);
          }else{
          item = item + object[key2] ;
          }
        }
      }
      items1.push(item);
      const listContainer = document.getElementById('list-container');
      const ul = document.createElement('ul');
      const li = document.createElement('li'); // Tworzenie <li> dla każdego elementu
      li.textContent = item; // Ustawienie tekstu w elemencie <li>
      li.id=key;
      // Dodanie obsługi zdarzenia 'click' dla każdego <li>
      li.addEventListener('click', function(event) {
        delItem(event.target.id);
      });
      ul.appendChild(li); // Dodawanie <li> do <ol>
      listContainer.appendChild(ul);
    }
  }    
)
function delItem(item){
  console.log(`Kliknięty element to: ${item}`);
  const url = new URL('http://192.168.100.7/del');
  url.searchParams.set('index', item);
  fetch(url)
  .then(response => {
    response.text();
    window.location.reload();
  })
}
function eraseEEprom(){
  if (confirm('Czy chcesz skasowac cały eeprom?')) {
    console.log( "erase eeprom");
    const url = new URL('http://192.168.100.7/erase');
    fetch(url)
    .then(response => {
      response.text();
      window.location.reload();
    })
  } else {
    console.log('EEprom nie został skasowany');
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
