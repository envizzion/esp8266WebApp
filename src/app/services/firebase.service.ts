import { Injectable } from '@angular/core';
import { AngularFireObject, AngularFireDatabase } from 'angularfire2/database';
import { Device } from '../models/Device';

@Injectable({
  providedIn: 'root'
})
export class FirebaseService {

  private dbPath = '/device1';
  private dbInfoPath = '/device1/info';

  // customersRef: AngularFireList<Customer> = null;

  deviceRef: AngularFireObject<Device> = null;
  infoRef:AngularFireObject<string> = null;
  
  constructor(private db: AngularFireDatabase) {
    this.deviceRef = db.object(this.dbPath);
    this.infoRef = db.object(this.dbInfoPath);

  }

getDeviceRef():AngularFireObject<Device>{
  return this.deviceRef;
}

getInfoRef():AngularFireObject<string>{
  return this.infoRef;
}





updateDevice(device:Device){
  
   this.deviceRef.update(device);
}

}

