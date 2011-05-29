
class Sprite
{
public:
    Sprite(const char* filename);
    void Draw(float x, float y, bool flipx=false, float angle=0.0f);
    void DrawSub(float x, float y, float subx, float suby, float subw, float subh);

    LPDIRECTDRAWSURFACE7 m_Surf;

    int m_Width;
    int m_Height;

};