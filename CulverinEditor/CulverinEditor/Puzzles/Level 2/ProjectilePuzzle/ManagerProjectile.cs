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

    // Ediatable Variables --------------------
    public float totaltime_puzzle = 30.0f;
    public float phase1 = 15.0f;
    public float phase2 = 10.0f;
    public float phase3 = 5.0f;


    private float actualtime_puzzle = 0.0f;



    public bool active_puzzle = false;
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


    }

    void Update()
    {
        if (active_puzzle)
        {

        }


    }
}