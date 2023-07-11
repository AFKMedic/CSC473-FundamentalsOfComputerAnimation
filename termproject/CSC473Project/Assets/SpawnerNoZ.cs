using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpawnerNoZ : MonoBehaviour
{
    public GameObject simulator_object;
    public GameObject particle_object;
    public float spawn_rate;
    public int max_particles = 1000;
    public float initial_x = 0.0f;
    public float initial_y = 0.0f;
    public float initial_z = 0.0f;
    private float time;

    // Start is called before the first frame update
    void Start()
    {
        simulator_object = GameObject.Find("Simulator");
        particle_object = GameObject.Find("Particle");
    }

    // Update is called once per frame
    void Update()
    {
        if (simulator_object.transform.childCount < max_particles)
        {
            time += Time.deltaTime;
            if (time < spawn_rate)
            {
                return;
            }

            GameObject particle = Instantiate(particle_object, transform.position, Quaternion.identity);
            particle.transform.parent = simulator_object.transform;

            particle.GetComponent<Particle>().position = transform.position;
            particle.GetComponent<Particle>().velocity = new Vector3(initial_x, initial_y, initial_z);

            time = 0.0f;
        }
    }
}
