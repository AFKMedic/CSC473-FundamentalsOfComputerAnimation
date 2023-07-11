using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class FPSCounter : MonoBehaviour
{
    public TextMeshProUGUI fps_counter;

    private float poll_rate = 0.2f;
    private float time;
    private int fps;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;
        if(time >= poll_rate)
        {
            fps = (int)(1/Time.deltaTime);
            fps_counter.text = "FPS: " + fps;
            time -= poll_rate;
        }
    }
}
