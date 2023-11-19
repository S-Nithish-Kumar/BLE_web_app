// Get references to UI elements

// Bluetooth connect and disconnect elements
let connectButton = document.getElementById('connect');
let disconnectButton = document.getElementById('disconnect');

// Led control elements
let no_alert_button = document.getElementById('no_alert');
let mild_alert_button = document.getElementById('mild_alert');
let high_alert_button = document.getElementById('high_alert');

// PSoC data subscribe and unsubscribe elements
let subscribe_button = document.getElementById('subscribe');
let unsubscribe_button = document.getElementById('unsubscribe');

// Form UI elements
let sendForm = document.getElementById('send-form');
let inputField = document.getElementById('input');



// initialize variables
let connectionKey = null
let characteristicCache1 = null;
let characteristicCache2 = null;
let characteristicCache3 = null;
let characteristicCache4 = null;
let deviceCache = null;



// Handle connectButton click event.
connectButton.addEventListener('click', function () {
  connectionKey = connect() // Connect to the device on Connect button click
  publishData(connectionKey);
  subscribeData(connectionKey);
  subscribeDataFlag(connectionKey);
  FormData(connectionKey);
  console.log("completed connecting");
});

// Launch Bluetooth device chooser and connect to the selected device
function connect() {
  return (deviceCache ? Promise.resolve(deviceCache) :
    requestBluetoothDevice()).
    then(device => connectDeviceAndCacheCharacteristic(device)).
    catch(error => log(error));
}

// Search BLE devices and assign device to deviceCache
function requestBluetoothDevice() {
  log('Requesting bluetooth device...');

  return navigator.bluetooth.requestDevice({
    filters: [{
      name: 'PSoC Controller'
    }],
    optionalServices: ['00004ba4-0000-1000-8000-00805f9b34fb'] // Required to access service later.
  }).
    then(device => {
      log('"' + device.name + '" bluetooth device selected');
      deviceCache = device;
      return deviceCache;
    });
}

// Connect to the device specified, get service
function connectDeviceAndCacheCharacteristic(device) {
  if (device.gatt.connected && characteristicCache1) {
    return Promise.resolve(characteristicCache1);
  }

  log('Connecting to GATT server...');

  return device.gatt.connect().
  then(server =>{
    log("Connected, getting primary service")
    return server?.getPrimaryService(0x4BA4);
  });
}


// Publish data to PSoC
publishData = (connectionKey) => {
  connectionKey.then(service => {
      log('Service found, getting characteristic one');
      return service.getCharacteristic(0x75A1);
    }).
    then(characteristic => {
      log('Characteristic one found');
      characteristicCache1 = characteristic;

      return characteristicCache1;
    });
}

// Subscribe data to PSoC
subscribeData = (connectionKey) => {
  connectionKey.then(service => {
      log('Service found, getting characteristic two');
      return service.getCharacteristic(0xF945);
    }).
    then(characteristic2 => {
      log('Characteristic two found');
      characteristicCache2 = characteristic2;
      return characteristicCache2;
    }).
    then(characteristic2 => {
      characteristic2.addEventListener('characteristicvaluechanged', (e) => { handleCharacteristicValueChanged(e, characteristic2) });
      return characteristic2.readValue();
    });
}

// Data receiving
function handleCharacteristicValueChanged(event, data) {
  //data?.readValue()
  let myData = document.getElementById("dataFromBle")
  myData.innerHTML = dataViewToDecimal(event.target.value)
}

// convert dataView format to decimal
function dataViewToDecimal(dataView) {
  let decimal = 0;
  let factor = 1;
  for (let i = dataView.byteLength - 1; i >= 0; i--) {
    const byte = dataView.getUint8(i);
    decimal += byte * factor;
    factor *= 256; // Multiply by 256 for each byte (2^8)
  }
  return decimal;
}



// Flag to subscribe data from PSoC
subscribeDataFlag = (connectionKey) => {
  connectionKey.then(service => {
      log('Service found, getting characteristic three');
      return service.getCharacteristic(0x12F7);
    }).
    then(characteristic3 => {
      log('Characteristic three found');
      characteristicCache3 = characteristic3;
      return characteristicCache3;
    });
}



// Send form data to PSoC
FormData = (connectionKey) => {
  connectionKey.then(service => {
      log('Service found, getting characteristic four');
      return service.getCharacteristic(0x507F);
    }).
    then(characteristic4 => {
      log('Characteristic four found');
      log('"' + deviceCache.name + '" bluetooth device connected');
      characteristicCache4 = characteristic4;
      return characteristicCache4;
    });
}


// Disconnect from the device on Disconnect button click
// call disconnect function
disconnectButton.addEventListener('click', function () {
  disconnect();
});

// disconnect from the BLE device
function disconnect() {
  if (deviceCache) {
    log('Disconnecting from "' + deviceCache.name + '" bluetooth device...');

    if (deviceCache.gatt.connected) {
      deviceCache.gatt.disconnect();
      log('"' + deviceCache.name + '" bluetooth device disconnected');
    }
    else {
      log('"' + deviceCache.name +
        '" bluetooth device is already disconnected');
    }
  }

  // If characteristicCache2 is valid, this line removes the event listener 
  // for the 'characteristicvaluechanged' event. The handleCharacteristicValueChanged 
  // function is the callback function that was previously registered to handle this event.
  if (characteristicCache2) {
    characteristicCache2.removeEventListener('characteristicvaluechanged',
        handleCharacteristicValueChanged);
    characteristicCache2 = null;
  }

  // set the deviceCache and the characteristic caches to null
  deviceCache = null;
  characteristicCache1 = null;
  characteristicCache3 = null;
  characteristicCache4 = null;
}



// Call no_alert_send function
no_alert_button.addEventListener('click', function () {
  no_alert_send();
});

function no_alert_send() {
  data = String('a');
  writeToCharacteristic(characteristicCache1, data);
  log('Sent "a" to BLE device');
}



// Call mild_alert_send function
mild_alert_button.addEventListener('click', function () {
  mild_alert_send();
});

function mild_alert_send() {
  data = String('b');
  writeToCharacteristic(characteristicCache1, data);
  log('Sent "b" to BLE device');
}



// Call high_alert_send function
high_alert_button.addEventListener('click', function () {
  high_alert_send();
});

function high_alert_send() {
  data = String('c');
  writeToCharacteristic(characteristicCache1, data);
  log('Sent "c" to BLE device');
}



// call subscribe_send function on the click event of subscribe_button
subscribe_button.addEventListener('click', function () {
  subscribe_send();
});

function subscribe_send() {
  data = String('e');
  writeToCharacteristic(characteristicCache3, data);
  log('Subscription flag ON');
}



// call unsubscribe_send function on the click event of unsubscribe_button
unsubscribe_button.addEventListener('click', function () {
  unsubscribe_send();
});

function unsubscribe_send() {
  data = String('f');
  writeToCharacteristic(characteristicCache3, data);
  log('Subscription flag OFF');
}



// Handle form submit event
sendForm.addEventListener('submit', function (event) {
  event.preventDefault(); // Prevent form sending
  send(inputField.value); // Send text field contents
  inputField.value = '';  // Zero text field
  inputField.focus();     // Focus on text field
});

function send(data) {
  data = String(data);

  // check the presence of data and characteristic
  if (!data || !characteristicCache4) {
    return;
  }

  writeToCharacteristic(characteristicCache4, data);
  log('Sent ' + `'${data}'` + ' to BLE device');
}



// Output to terminal
function log(data) {
  document.getElementById('datalog').innerText = data
}



// Write to the BLE device with the specific characteristic without response
function writeToCharacteristic(characteristic, data) {
  //log(data);
  characteristic.writeValueWithoutResponse(new TextEncoder().encode(data));
}










