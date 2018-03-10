using CulverinEditor;

class Test : CulverinBehaviour
{
    CompMaterial material = null;

    void Start()
    {
        material = GetComponent<CompMaterial>();
    }

    void Update()
    {
        if(Input.GetInput_KeyDown("FinalSkill","Player"))
        {
            material.SetAlbedo("Shield.png");
        }
    }
}

