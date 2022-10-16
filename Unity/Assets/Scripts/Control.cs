using System;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using UnityEngine;

public class Control : MonoBehaviour
{
    SerialPort _serialPort;
    float nx, ny, nz;
    byte[] buffer = new byte[12];
    
    void Start()
    {
        _serialPort = new SerialPort();
        _serialPort.PortName = "/dev/ttyUSB0";
        _serialPort.BaudRate = 115200;
        _serialPort.DtrEnable = true;
        _serialPort.Open();
        Debug.Log("Open Serial Port");
    }

    // Update is called once per frame
    void Update()
    {
        _serialPort.Write("s");
        
        if (_serialPort.BytesToRead >= 12)
        {
            Debug.Log("rec");
            _serialPort.Read(buffer, 0, 12);
            nz = BitConverter.ToSingle(buffer, 0);
            nx = - BitConverter.ToSingle(buffer, 4);
            ny = BitConverter.ToSingle(buffer, 8);
        }
        transform.rotation = Quaternion.Euler(nx,ny,nz);
        
    }
}

