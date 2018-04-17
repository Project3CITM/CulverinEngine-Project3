using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class DamageFeedback: CulverinBehaviour
{
    Material mat;

    private bool damage = false;
    private float alpha = 1.0f;

    //Duration of the blood texture at the screen
    public float time = 2.0f;

    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        damage = false;
        alpha = 0;
    }

    void Update()
    {
        //if (Input.GetKeyDown(KeyCode.Num9))
        //{
        //    alpha = 1.0f;
        //    damage = true;
        //}

        if (alpha > 0.0f)
        {
            float decrease_val = Time.deltaTime / time;
            alpha -= decrease_val;
            Debug.Log(alpha, Department.PLAYER, Color.BLUE);
        }

        if (alpha <= 0.0f)
        {
            damage = false;
        }

        mat.SetBool("damage", damage);
        mat.SetFloat("alpha", alpha);
    }

    //Depending on the hp left, set more alpha
    public void SetDamage(float curr_hp, float max_hp)
    {
        float result = curr_hp / max_hp;
        alpha = 1 - result;
        damage = true;
        Debug.Log(curr_hp + " " + max_hp + " " + result, Department.PLAYER, Color.NONE);
    }
}