using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Drop : MonoBehaviour
{
    public GameObject simulator_object;
    public GameObject particle_object;
    public float length = 10;
    public float height = 25;
    public float depth = 10;
    // Start is called before the first frame update
    void Start()
    {
        simulator_object = GameObject.Find("Simulator");
        particle_object = GameObject.Find("Particle");

        for(int i = 0; i < length; i++)
        {
            for (int j = 0; j < height; j++)
            {
                for (int k = 0; k < depth; k++)
                {
                    Vector3 location = new Vector3(0.2f + (i * 0.2f), 0.1f + (j * 0.2f), 0.2f + (k * 0.2f));
                    GameObject particle = Instantiate(particle_object, location, Quaternion.identity);
                    particle.transform.parent = simulator_object.transform;
                    particle.GetComponent<Particle>().position = location;
                }
            }
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
