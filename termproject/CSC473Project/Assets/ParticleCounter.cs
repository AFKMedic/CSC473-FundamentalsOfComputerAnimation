using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ParticleCounter : MonoBehaviour
{
    public TextMeshProUGUI particle_counter;
    public GameObject simulator_object;

    private float poll_rate = 0.2f;
    private float time;
    private int num_particles;

    // Start is called before the first frame update
    void Start()
    {
        simulator_object = GameObject.Find("Simulator");
    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;
        if (time >= poll_rate)
        {
            num_particles = simulator_object.transform.childCount;
            particle_counter.text = "Particles: " + num_particles;
            time -= poll_rate;
        }
    }
}
