namespace CulverinEditor
{
    public class float4x4
    {
        public float4x4()
        {
            a0 = 1.0f;
            a1 = 0.0f;
            a2 = 0.0f;
            a3 = 0.0f;

            b0 = 0.0f;
            b1 = 1.0f;
            b2 = 0.0f;
            b3 = 0.0f;

            c0 = 0.0f;
            c1 = 0.0f;
            c2 = 1.0f;
            c3 = 0.0f;

            d0 = 0.0f;
            d1 = 0.0f;
            d2 = 0.0f;
            d3 = 1.0f;
        }

        public float4x4(float4x4 cpy)
        {
           this.a0 = cpy.a0;
           this.a1 = cpy.a1;
           this.a2 = cpy.a2;
           this.a3 = cpy.a3;
           
           this.b0 = cpy.b0;
           this.b1 = cpy.b1;
           this.b2 = cpy.b2;
           this.b3 = cpy.b3;
           
           this.c0 = cpy.c0;
           this.c1 = cpy.c1;
           this.c2 = cpy.c2;
           this.c3 = cpy.c3;
           
           this.d0 = cpy.d0;
           this.d1 = cpy.d1;
           this.d2 = cpy.d2;
           this.d3 = cpy.d3;
        }

        public override string ToString()
        {
            string temp = "\n(" + a0.ToString() + ", " + a1.ToString() + ", " + a2.ToString() + ", " + a3.ToString() + ")\n(" + b0.ToString() + ", " + b1.ToString() + ", " + b2.ToString() + ", " + b3.ToString() + ")\n(" + c0.ToString() + ", " + c1.ToString() + ", " + c2.ToString() + ", " + c3.ToString() + ")\n(" + d0.ToString() + ", " + d1.ToString() + ", " + d2.ToString() + ", " + d3.ToString() + ")";
            return temp;
        }

        public float a0, a1, a2, a3;
        public float b0, b1, b2, b3;
        public float c0, c1, c2, c3;
        public float d0, d1, d2, d3;
    }


}