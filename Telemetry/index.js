  // 1. Get currentValues from DB as JS object
  // 2. For each element in table
  // 3. Set the value according to what's in the JS object
    // Initialize Firebase
    var config = {
      apiKey: "AIzaSyA6B5wo3ClUr0ExzklEU_V6YEfRwrlVd2M",
      authDomain: "hytech-telemetry.firebaseapp.com",
      databaseURL: "https://hytech-telemetry.firebaseio.com",
      projectId: "hytech-telemetry",
      storageBucket: "hytech-telemetry.appspot.com",
      messagingSenderId: "464987762817"
    };
    firebase.initializeApp(config);

firebase.database().ref('/canBus/currentValues').once('value').then(function (snapshot) {
    var currentValues = snapshot.val();
    console.log(currentValues);
    for (key in currentValues) {
        if (!document.getElementById(key)) {
          console.log('missing: ' + key)
          continue;
        }
        document.getElementById(key).innerHTML = currentValues[key];
    }
});

