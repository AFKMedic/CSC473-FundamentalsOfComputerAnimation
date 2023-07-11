using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Particle : MonoBehaviour
{
    public Vector3 position;
    public float density = 0.0f;
    public float pressure = 0.0f;
    public List<Particle> neighbours = new List<Particle>();
    public Vector3 velocity;
    public Vector3 force;
    public int cell_x;
    public int cell_y;
    public int cell_z;
    public float mass = 1.0f;
   
    // Start is called before the first frame update
    void Start()
    {
        density = 0.0f;
        position = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void DestroyParticle()
    {
        Destroy(gameObject);
    }

    // Utilize unity collision detection to push particles in bounds
    void OnCollisionStay(Collision collision)
    {
        // Check if moving away from object and return if true
        Vector3 normal = Vector3.zero;

        normal = collision.GetContact(0).normal;
        float vel_normal = Vector3.Dot(velocity, normal);
        if (vel_normal > 0)
        {
            return;
        }

        // Dampen veclocity and move out of obstacle
        Vector3 vel_tangent = velocity - normal * vel_normal;
        velocity = vel_tangent - normal * vel_normal * (0.3f);
        position = collision.contacts[0].point + normal * 0.1225f;
    }
}
