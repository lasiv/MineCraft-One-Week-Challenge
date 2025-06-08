#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>

class RenderMaster;

class InvMainWindow
{
    public:

        InvMainWindow();

        ~InvMainWindow();

        void draw(RenderMaster &renderer);

    private:
        u_int m_height;
        u_int m_width;
        u_int m_posx;
        u_int m_posy;

        std::vector<InvFrame> m_frames;
};

class InvFrame
{

};

class InvItem
{

};

#endif // INVENTORY_H_INCLUDED