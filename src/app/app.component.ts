import { Component } from '@angular/core';
import { FirebaseService } from './services/firebase.service';
import { Observable } from 'rxjs';
import { Device } from './models/Device';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent {
  title = 'esp8266-iot';
  checkBox:boolean;
  message:string;
  prevInfo:string;
  deviceObs: Observable<Device>;
  infoObs: Observable<string>;

  device:Device;
  constructor(private  firebaseService:FirebaseService){

  }

  ngOnInit(){
    this.checkBox = true;
    this.message="";
    this.device = new Device();
    console.log(this.checkBox)

   this.deviceObs = this.firebaseService.getDeviceRef().valueChanges();
   this.infoObs = this.firebaseService.getInfoRef().valueChanges();
 

   this.deviceObs.subscribe(val =>{
         this.device = val;
         this.checkBox=(this.device.web_switch==1)?true:false;
        
   })

   this.infoObs.subscribe(val =>{


    this.message= "Last RFID detected : "+ val ;

    // if(this.message ==""){
    //   this.message=val;

    // }
    // else if(this.message =="" && !val.includes("New")  ){
    //   this.message= "New RFID detected : "+ val ;
    //   this.device.info = this.message;
    //   this.firebaseService.updateDevice(this.device);      
      
    // }

    
    console.log(val)

 
    
})

  }

  ngAfterInit(){
    console.log(this.checkBox)
  }

  check($event){
    this.checkBox = $event;
    this.device.web_switch = this.checkBox?1:0;

    this.firebaseService.updateDevice(this.device);

    console.log(this.checkBox)


}

close(){
  
}


}