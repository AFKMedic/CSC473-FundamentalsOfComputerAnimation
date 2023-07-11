using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Diagnostics;

public class Simulator : MonoBehaviour
{
    public List<Particle> particles = new List<Particle>();
    public GameObject particle_object;

    // Timing vars
    //--------------------------------------------------------------------
    //Stopwatch st = new Stopwatch();
    //int iter = 1;
    //float neighbour_time = 0;
    //float neighbour_average = 0;
    //float pressure_time = 0;
    //float pressure_average = 0;
    //float pressure_force_time = 0;
    //float pressure_force_average = 0;
    //float visc_time = 0;
    //float visc_average = 0;
    //float externel_time = 0;
    //float externel_average = 0;
    //float update_time = 0;
    //float update_average = 0;
    //--------------------------------------------------------------------

    // Grid Vars
    //--------------------------------------------------------------------
    public int grid_width   = 60;
    public int grid_height  = 60;
    public int grid_depth   = 60;
    public List<Particle>[,,] grid;
    public float x_min = -1.0f;
    public float y_min = -1.0f;
    public float z_min = -1.0f;
    public float x_max = 60.0f;
    public float y_max = 60.0f;
    public float z_max = 60.0f;
    //--------------------------------------------------------------------

    // Physics Vars
    //--------------------------------------------------------------------
    public float K = 10.0f;         // Pressure Constant
    public float h = 0.3f;          // Smoothing Radius
    public float R = 0.4f;          // Neighbour Radius
    public float time_step = 1.8f;
    public float rest_density = 0.1f;
    public float epsilon = 2.5f;
    public float gravity = 9.8f;
    //--------------------------------------------------------------------

    // Start is called before the first frame update
    void Start()
    {
        particle_object = GameObject.Find("Particle");
        grid = new List<Particle>[grid_width, grid_height, grid_depth];
        for (int i = 0; i < grid_width; i++)
        {
            for (int j = 0; j < grid_height; j++)
            {
                for (int k = 0; k < grid_depth; k++)
                {
                    grid[i, j, k] = new List<Particle>();
                }
            }
        }
    }

    // Smoothing Kernels
    //--------------------------------------------------------------------
    float Poly6(float r)
    {
        float i_exp = 1.6f;
        float o_exp = 4.0f;
        return (r <= h) ? (315 / (64 * 3.14f * Mathf.Pow(h, 9))) * Mathf.Pow((Mathf.Pow(h, i_exp) - Mathf.Pow(r, i_exp)), o_exp) : 0.0f;
    }

    float PolyCustom(float r)
    {
        return (r <= h) ? (45 / (3.14f * Mathf.Pow(h, 6))) * Mathf.Pow(((h - r)), 3) : 0.0f;
    }

    float ViscLapalacian(float r)
    {
        return (r <= h) ? ((45) / (3.14f * Mathf.Pow(h, 6))) * (h - r) : 0.0f;

    }
    //--------------------------------------------------------------------


    // Physics
    //--------------------------------------------------------------------
    // Calculate the density and add neigbouring particles to a neighbours list
    // for future calculations
    // For some reason unity refuses to run with a triple nested for loop to check
    // the current and surrounding cells so it is currently set up to calculate the
    // the density and neighbours in 3 double nested for loops. One for each Z value
    // that needs to be checked
    public void LocalDensity()
    {
        foreach (Particle p in particles)
        {
            int z_slice_1 = p.cell_z - 1;
            int z_slice_2 = p.cell_z;
            int z_slice_3 = p.cell_z + 1;
            for (int i = p.cell_x - 1; i <= p.cell_x + 1; i++)
            {
                for (int j = p.cell_y - 1; j <= p.cell_y + 1; j++)
                {
                    if (i >= 0 && i < grid_width && j >= 0 && j < grid_height && z_slice_1 >= 0 && z_slice_1 < grid_depth)
                    {
                        foreach (Particle n in grid[i, j, z_slice_1])
                        {
                            float dist = Vector3.Distance(p.position, n.position);
                            if (dist < R)
                            {
                                float W = Poly6(dist);
                                p.density += W;
                                if (dist > 0)
                                {
                                    p.neighbours.Add(n);
                                }
                            }
                        }
                    }
                }
            }

            for (int i = p.cell_x - 1; i <= p.cell_x + 1; i++)
            {
                for (int j = p.cell_y - 1; j <= p.cell_y + 1; j++)
                {
                    if (i >= 0 && i < grid_width && j >= 0 && j < grid_height && z_slice_2 >= 0 && z_slice_2 < grid_depth)
                    {
                        foreach (Particle n in grid[i, j, z_slice_2])
                        {
                            float dist = Vector3.Distance(p.position, n.position);
                            if (dist < R)
                            {
                                float W = Poly6(dist);
                                p.density += W;
                                if (dist > 0)
                                {
                                    p.neighbours.Add(n);
                                }
                            }
                        }
                    }
                }
            }

            for (int i = p.cell_x - 1; i <= p.cell_x + 1; i++)
            {
                for (int j = p.cell_y - 1; j <= p.cell_y + 1; j++)
                {
                    if (i >= 0 && i < grid_width && j >= 0 && j < grid_height && z_slice_3 >= 0 && z_slice_3 < grid_depth)
                    {
                        foreach (Particle n in grid[i, j, z_slice_3])
                        {
                            float dist = Vector3.Distance(p.position, n.position);
                            if (dist < R)
                            {
                                float W = Poly6(dist);
                                p.density += W;
                                if (dist > 0)
                                {
                                    p.neighbours.Add(n);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Calculate a single particle's pressure
    public void CalculatePressure()
    {
        foreach (Particle p in particles)
        {
            p.pressure = (K * (p.density - rest_density));
        }
    }

    // Calculate the pressure forces applied to the particle
    public void PressureForce()
    {
        foreach (Particle p in particles)
        {
            Vector3 pressure_force = Vector3.zero;

            foreach (Particle n in p.neighbours)
            {
                float pressure = p.pressure + n.pressure;
                float den = 2 * n.density;
                float dis = (n.position - p.position).magnitude;
                Vector3 dir = (n.position - p.position).normalized;
                float W = PolyCustom(dis);
                pressure_force += n.mass * (pressure / den) * W * dir;
            }
            p.force -= pressure_force;
        }
    }

    // Calculate the viscosity forces applied to the particle
    public void ViscosityForce()
    {
        
        foreach (Particle p in particles)
        {
            Vector3 visc_force = Vector3.zero;
            foreach (Particle n in p.neighbours)
            {
                Vector3 vel = n.velocity - p.velocity;
                Vector3 dir = (n.position - p.position).normalized;
                float dis = (p.position - n.position).magnitude;
                float lapa = ViscLapalacian(dis);
                float den = (n.density > 0) ? n.density : 1;
                visc_force += n.mass * Vector3.Scale((vel / den) * lapa , dir);
            }
            visc_force *= epsilon;
            p.force += visc_force;
        }
    }

    // Add any extra external forces
    // in this case just gravity
    public void ExternalForce()
    {
        foreach (Particle p in particles)
        {
            p.force += new Vector3(0.0f, -gravity, 0.0f);
        }
    }
    //--------------------------------------------------------------------

    public void UpdateParticles()
    {
        foreach (Particle p in particles)
        {
            p.velocity += (p.force / p.density) * Time.deltaTime * time_step;
            p.position += p.velocity * Time.deltaTime * time_step;

            p.transform.position = p.position;

            // Destroy particles that somehow made their way out of bounds
            if (p.position.x < x_min || p.position.y < y_min || p.position.z < z_min || p.position.x > x_max || p.position.y > y_max || p.position.z > z_max)
            {
                // Keep original particle
                if (name != "Particle")
                {
                    p.DestroyParticle();
                }
            }

        }
    }

    // Update is called once per frame
    void Update()
    {
        // Assign particles to simulator
        particles.Clear();
        foreach (Transform child in transform)
        {
            particles.Add(child.GetComponent<Particle>());
        }

        // Clear all lists in the grid
        for (int i = 0; i < grid_width; i++)
        {
            for (int j = 0; j < grid_height; j++)
            {
                for (int k = 0; k < grid_depth; k++)
                {
                    grid[i, j, k].Clear();
                }
            }
        }

        
        foreach (Particle p in particles)
        {
            p.density = 0.0f;
            p.force = Vector3.zero;
            p.neighbours = new List<Particle>();

            // Take global postion and assign grid cell
            p.cell_x = Mathf.FloorToInt((p.position.x - x_min) / (x_max - x_min) * grid_width);
            p.cell_y = Mathf.FloorToInt((p.position.y - y_min) / (y_max - y_min) * grid_height);
            p.cell_z = Mathf.FloorToInt((p.position.z - z_min) / (z_max - y_min) * grid_depth);

            // Add particle to respective grid cell
            if (p.cell_x >= x_min && p.cell_x < grid_width && p.cell_y >= y_min && p.cell_y < grid_height && p.cell_z >= z_min && p.cell_z < grid_depth)
            {
                grid[p.cell_x, p.cell_y, p.cell_z].Add(p);
            }
        }

        
        //st.Start();
        LocalDensity();
        //st.Stop();
        //neighbour_time += st.ElapsedTicks;
        //neighbour_average = neighbour_time / iter;
        //UnityEngine.Debug.Log(string.Format("Neighbour and Density took {0} ticks to complete", neighbour_average));
        //st.Reset();

        //st.Start();
        CalculatePressure();
        //st.Stop();
        //pressure_time += st.ElapsedTicks;
        //pressure_average = pressure_time / iter;
        //UnityEngine.Debug.Log(string.Format("Pressure took {0} ticks to complete", pressure_average));
        //st.Reset();

        //st.Start();
        PressureForce();
        //st.Stop();
        //pressure_force_time += st.ElapsedTicks;
        //pressure_force_average = pressure_force_time / iter;
        //UnityEngine.Debug.Log(string.Format("Pressure Force took {0} ticks to complete", pressure_force_average));
        //st.Reset();

        //st.Start();
        ViscosityForce();
        //st.Stop();
        //visc_time += st.ElapsedTicks;
        //visc_average = visc_time / iter;
        //UnityEngine.Debug.Log(string.Format("Visc Force took {0} ticks to complete", visc_average));
        //st.Reset();

        //st.Start();
        ExternalForce();
        //st.Stop();
        //externel_time += st.ElapsedTicks;
        //externel_average = externel_time / iter;
        //UnityEngine.Debug.Log(string.Format("External Force took {0} ticks to complete", externel_average));
        //st.Reset();

        //st.Start();
        UpdateParticles();
        //st.Stop();
        //update_time += st.ElapsedTicks;
        //update_average = update_time / iter;
        //UnityEngine.Debug.Log(string.Format("Particle Update took {0} ticks to complete", update_average));
        //st.Reset();

        //iter++;
    }
}
