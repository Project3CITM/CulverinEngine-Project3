using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class DamageFeedback: CulverinBehaviour
{
    Material mat;
    bool damage;
    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        damage = false;
    }
    void Update()
    {
        mat.SetBool("damage", damage);

        if (Input.GetKeyDown(KeyCode.Q))
            damage = !damage;

      
    }


}