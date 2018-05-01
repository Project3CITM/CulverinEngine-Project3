using CulverinEditor;
using CulverinEditor.Debug;
using System.Collections;
using System.Collections.Generic;

public class TestParticlesV2 : CulverinBehaviour
{
    public GameObject parent_particles;
    List<GameObject> particles;
    void Start()
    {
        parent_particles = GetLinkedObject("parent_particles");
        particles = new List<GameObject>();
        for (int i = 0; i< parent_particles.ChildCount(); i++)
        {
            particles.Add(parent_particles.GetChildByTagIndex("weapon", i));
        }
    }

    void Update()
    {
        if (Input.GetKeyUp(KeyCode.E))
        {
            for (int i = 0; i < particles.Count; i++)
            {
                particles[i].GetComponent<CompParticleSystem>().ActivateEmission(false);
            }
        }
        if (Input.GetKeyDown(KeyCode.E))
        {
            for (int i = 0; i < particles.Count; i++)
            {
                particles[i].GetComponent<CompParticleSystem>().ActivateEmission(true);
            }
        }
    }
}