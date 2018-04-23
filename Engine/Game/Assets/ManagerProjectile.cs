using System.Collections;
using System.Collections.Generic;
using CulverinEditor;
using CulverinEditor.Debug;

public class ManagerProjectile : CulverinBehaviour
{
    // Lines-----------------------------
    public GameObject projectileOut_1;
    public GameObject projectileOut_2;
    public GameObject projectileOut_3;
    public GameObject projectileOut_4;
    public GameObject projectileOut_5;
    public GameObject projectileOut_6;
    public GameObject projectileOut_7;
    public GameObject projectileOut_8;

    public GameObject door_puzzle_level2;

    // Ediatable Variables --------------------
    public int number_projectils = 8;

    public float speed_projectil = 65.0f;

    public float totaltime_puzzle = 30.0f;
    public float phase1 = 15.0f;
    public float phase2 = 10.0f;
    public float phase3 = 5.0f;

    public float phase1_projects_per_time = 1.0f;
    public float phase2_projects_per_time = 0.65f;
    public float phase3_projects_per_time = 0.40f;

    private float actualtime_puzzle = 0.0f;
    public float actualtime_projectils = 0.0f;



    public bool active_puzzle = false;
    private int phase_active = 0;
    // --------------------------------------

    void Start()
    {
        projectileOut_1 = GetLinkedObject("projectileOut_1");
        projectileOut_2 = GetLinkedObject("projectileOut_2");
        projectileOut_3 = GetLinkedObject("projectileOut_3");
        projectileOut_4 = GetLinkedObject("projectileOut_4");
        projectileOut_5 = GetLinkedObject("projectileOut_5");
        projectileOut_6 = GetLinkedObject("projectileOut_6");
        projectileOut_7 = GetLinkedObject("projectileOut_7");
        projectileOut_8 = GetLinkedObject("projectileOut_8");

        door_puzzle_level2 = GetLinkedObject("door_puzzle_level2");

    }

    void Update()
    {
        if (active_puzzle)
        {
            if(door_puzzle_level2.GetComponent<DoorLevel2>().door_closed && phase_active == 0)
            {
                phase_active = 1;
            }

            if (phase_active > 0)
            {
                actualtime_puzzle += Time.deltaTime;
            }

            if (phase_active == 1 && actualtime_puzzle >= phase1)
            {
                phase_active = 2;
            }
            else if (phase_active == 2 && actualtime_puzzle >= phase1 + phase2)
            {
                phase_active = 3;
            }
            else if (phase_active == 3 && actualtime_puzzle >= phase1 + phase2 + phase3)
            {
                active_puzzle = false;
                phase_active = -1;
            }

            switch (phase_active)
            {
                case 1:
                    {
                        if (actualtime_projectils >= phase1_projects_per_time)
                        {
                            ApplyProjectil(Random.Range(0, number_projectils + 1));

                        }
                        else
                        {
                            actualtime_projectils += Time.deltaTime;
                        }
                        break;
                    }
                case 2:
                    {
                        if (actualtime_projectils >= phase1_projects_per_time)
                        {
                            ApplyProjectil(Random.Range(0, number_projectils + 1));

                        }
                        else
                        {
                            actualtime_projectils += Time.deltaTime;
                        }
                        break;
                    }
                case 3:
                    {
                        if (actualtime_projectils >= phase1_projects_per_time)
                        {
                            ApplyProjectil(Random.Range(0, number_projectils + 1));

                        }
                        else
                        {
                            actualtime_projectils += Time.deltaTime;
                        }
                        break;
                    }
            }
        }
    }

    void ApplyProjectil(int projectil)
    {
        switch (projectil)
        {
            case 1:
                {
                    if (projectileOut_1 != null && number_projectils >= 1)
                    {
                        Shoot(projectileOut_1);
                    }
                    break;
                }
            case 2:
                {
                    if (projectileOut_2 != null && number_projectils >= 2)
                    {
                        Shoot(projectileOut_2);
                    }
                    break;
                }
            case 3:
                {
                    if (projectileOut_3 != null && number_projectils >= 3)
                    {
                        Shoot(projectileOut_3);
                    }
                    break;
                }
            case 4:
                {
                    if (projectileOut_4 != null && number_projectils >= 4)
                    {
                        Shoot(projectileOut_4);
                    }
                    break;
                }
            case 5:
                {
                    if (projectileOut_5 != null && number_projectils >= 5)
                    {
                        Shoot(projectileOut_5);
                    }
                    break;
                }
            case 6:
                {
                    if (projectileOut_6 != null && number_projectils >= 6)
                    {
                        Shoot(projectileOut_6);
                    }
                    break;
                }
            case 7:
                {
                    if (projectileOut_7 != null && number_projectils >= 7)
                    {
                        Shoot(projectileOut_7);
                    }
                    break;
                }
            case 8:
                {
                    if (projectileOut_8 != null && number_projectils >= 8)
                    {
                        Shoot(projectileOut_8);
                    }
                    break;
                }
        }
    }

    void Shoot(GameObject projectileOut)
    {
        //PlayFx("audioooo");
        GameObject arrow = Instantiate("Projectil");
        arrow.transform.SetPosition(projectileOut.GetComponent<Transform>().GetGlobalPosition());
        arrow.transform.SetRotation(projectileOut.GetComponent<Transform>().GetRotation());

        Arrow arrow_script = arrow.GetComponent<Arrow>();
        Vector3 vfront = projectileOut.GetComponent<Transform>().forward;
        Mathf.Round(vfront.x);
        Mathf.Round(vfront.y);
        Mathf.Round(vfront.z);
        arrow_script.speed = vfront;
    }

    void OnTriggerEnter()
    {
        // Close door
        door_puzzle_level2.GetComponent<DoorLevel2>().CloseDoor();
        active_puzzle = true;
    }
}