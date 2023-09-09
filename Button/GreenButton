using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GreenButton : MonoBehaviour
{
    public Image imageColor;
    public Color color1,color2;
    mqttReceiver mqttreceiver;
    public mqttReceiver receiver;

    public bool isChanged =  false;

    public void colorchange()
    {
        mqttreceiver = receiver.GetComponent<mqttReceiver>();

        mqttreceiver.topicPublish = "emqx/esp32/led";

        if(isChanged == false)
        {
            imageColor.color = color1;
            isChanged = true;
            
            mqttreceiver.messagePublish = "green-on";
            mqttreceiver.Publish();
        }
        else if(isChanged == true)
        {
            imageColor.color = color2;
            isChanged = false;
            
            mqttreceiver.messagePublish = "green-off";
            mqttreceiver.Publish();
        }
        
    }

    
}
