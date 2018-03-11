using CulverinEditor;
using CulverinEditor.Debug;


    public class Test : CulverinBehaviour
    {
        public CompMaterial material = null;

        void Start()
        {
            material = GetComponent<CompMaterial>();
        }

    void Update()
    {

        if (Input.GetInput_KeyDown("FinalSkill", "Player"))
        {
            material.SetAlbedo("Shield.png");
            material.SetAmbientOcclusion("Shield.png");
            material.SetNormals("Shield.png");
        }
    }
}


