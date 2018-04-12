using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class DamageFeedback: CulverinBehaviour
{
    Material mat;
    bool damage;
    float alpha;
    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        damage = false;
        alpha = 0;
    }
    void Update()
    {
        mat.SetBool("damage", damage);
        mat.SetFloat("alpha", alpha);
        if (Input.GetKeyDown(KeyCode.Q))
        {
            alpha = 1.0f;
            damage = true;
        }

        if (alpha > 0.0f)
            alpha -= Time.deltaTime ;
      
        if(alpha <= 0.0f)
        {
            damage = false;
        }
    }


}