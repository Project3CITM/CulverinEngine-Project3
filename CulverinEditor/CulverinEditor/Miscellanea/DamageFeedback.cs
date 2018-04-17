using CulverinEditor;
using CulverinEditor.Debug;
using CulverinEditor.Map;

public class DamageFeedback: CulverinBehaviour
{
    Material mat;

    private bool damage = false;
    private float alpha = 1.0f;
    bool dead = false;
    bool reviving = false;
    float mult_dead = 1.0f;
    //Duration of the blood texture at the screen
    public float time = 2.0f;
    public float dying_time = 2.0f;
    public float dead_time = 1.0f;
    float curr_dead_time = 0.0f;

    void Start()
    {
        mat = GetMaterialByName("Final Tex Material");
        damage = false;
        dead = false;
        reviving = true;
        alpha = 0;
        curr_dead_time = 0.0f;
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

       
        if (dead)
        {
            if (mult_dead > 0.0f)
            {
                float decrease_val = Time.deltaTime / dying_time;
                mult_dead -= decrease_val;
            }    
        }
        if (mult_dead <= 0.0f)
        {
            dead = false;
            reviving = true;
        }
        if (reviving)
        {
            curr_dead_time += Time.deltaTime;
            if (curr_dead_time >= dead_time)
            {
                float decrease_val = Time.deltaTime / dying_time;
                mult_dead += decrease_val;
                if (mult_dead >= 1)
                {
                    mult_dead = 1;
                    reviving = false;
                }
            }
        }

        if (alpha <= 0.0f)
        {
            damage = false;
        }

        mat.SetBool("damage", damage);
        mat.SetFloat("alpha", alpha);
        mat.SetFloat("mult_dead", mult_dead);
    }

    //Depending on the hp left, set more alpha
    public void SetDamage(float curr_hp, float max_hp)
    {
        float result = curr_hp / max_hp;
        alpha = 1 - result;
        damage = true;
        Debug.Log(curr_hp + " " + max_hp + " " + result, Department.PLAYER, Color.NONE);
    }

    public void CharacterDie()
    {
        dead = true;
        curr_dead_time = 0.0f;
        reviving = false;
    }
}