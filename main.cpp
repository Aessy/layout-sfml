#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>

#define LAY_IMPLEMENTATION
#define LAY_FLOAT 1
#include "layout.h"

#include <iostream>
#include <vector>

struct layout
{
    lay_context ctx;
    lay_id root;
    lay_id title;
    lay_id splits;
    std::vector<lay_id> rows;
    lay_id timer;
    lay_id extra;

    void run()
    {
        lay_run_context(&ctx);

        lay_vec4 vec4 = lay_get_rect(&ctx, title);
        std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';

        for (auto const& row : rows)
        {
            vec4 = lay_get_rect(&ctx, row);
            std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';
        }

        vec4 = lay_get_rect(&ctx, timer);
        std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';

        vec4 = lay_get_rect(&ctx, extra);
        std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';
    }

    void resize(size_t size_x, size_t size_y)
    {
        float title_space_prc = 10;
        float splits_space_prc = 60;
        float timer_space_prc = 10;
        float extra_space_prc = 20;

        float title_height = size_y / 100.0f * title_space_prc;
        float splits_height = size_y / 100.0f * splits_space_prc;
        float timer_height = size_y / 100.0f * timer_space_prc;
        float extra_height = size_y / 100.0f * extra_space_prc;

        std::cout << title_height + splits_height + timer_height + extra_height << "\n\n";

        lay_set_size_xy(&ctx, root, size_x, size_y);
        lay_set_size_xy(&ctx, title, 0, title_height);
        lay_set_size_xy(&ctx, splits, 0, splits_height);
        for (auto const& row : rows)
        {
            int const height = splits_height / rows.size();
            lay_set_size_xy(&ctx, row, 0, height);
        }

        lay_set_size_xy(&ctx, timer, 0, timer_height);
        lay_set_size_xy(&ctx, extra, 0, extra_height);
    }
};

layout create_layout(size_t size_x, size_t size_y)
{
    float title_space_prc = 10;
    float splits_space_prc = 60;
    float timer_space_prc = 10;
    float extra_space_prc = 20;

    float title_height = size_y / 100.0f * title_space_prc;
    float splits_height = size_y / 100.0f * splits_space_prc;
    float timer_height = size_y / 100.0f * timer_space_prc;
    float extra_height = size_y / 100.0f * extra_space_prc;

    lay_context ctx;
    lay_init_context(&ctx);

    lay_reserve_items_capacity(&ctx, 1024);

    lay_id root = lay_item(&ctx);

    lay_set_size_xy(&ctx, root, size_x, size_y);
    lay_set_contain(&ctx, root, LAY_COLUMN);

    // Title
    lay_id title = lay_item(&ctx);
    lay_set_size_xy(&ctx, title, 0, title_height);
    lay_set_behave(&ctx, title, LAY_HFILL);
    lay_set_contain(&ctx, title, LAY_COLUMN);
    lay_insert(&ctx, root, title);

    // Splits
    lay_id splits = lay_item(&ctx);
    lay_set_size_xy(&ctx, splits, 0, splits_height);
    lay_set_behave(&ctx, splits, LAY_HFILL);
    lay_set_contain(&ctx, splits, LAY_COLUMN);
    lay_insert(&ctx, root, splits);

    std::vector<int> childs { 1, 2, 3, 4, 5 };

    std::vector<lay_id> rows;
    for (auto const& child : childs)
    {
        lay_id row = lay_item(&ctx);
        lay_insert(&ctx, splits, row);
        lay_set_behave(&ctx, row, LAY_HFILL);

        int const height = splits_height / childs.size();
        lay_set_size_xy(&ctx, row, 0, height);

        rows.push_back(row);
    }

    lay_id timer = lay_item(&ctx);
    lay_set_size_xy(&ctx, timer, 0, timer_height);
    lay_set_behave(&ctx, timer, LAY_HFILL);
    lay_insert(&ctx, root, timer);

    lay_id extra = lay_item(&ctx);
    lay_set_size_xy(&ctx, extra, 0, extra_height);
    lay_set_behave(&ctx, extra, LAY_HFILL);
    lay_insert(&ctx, root, extra);

    lay_run_context(&ctx);

    lay_vec4 vec4 = lay_get_rect(&ctx, title);
    std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';

    for (auto const& row : rows)
    {
        vec4 = lay_get_rect(&ctx, row);
        std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';
    }

    vec4 = lay_get_rect(&ctx, timer);
    std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';

    vec4 = lay_get_rect(&ctx, extra);
    std::cout << vec4[0] << " " << vec4[1] << " "<< vec4[2] << " "<< vec4[3] << '\n';

    return layout { ctx, root, title, splits, rows, timer, extra };

    lay_reset_context(&ctx);
    lay_destroy_context(&ctx);
}

template<typename T>
struct ShapeWithLayout
{
    lay_id shape_layout;
    T shape;

    void update(layout const& layout)
    {
        auto const lay = lay_get_rect(&layout.ctx, shape_layout);
        shape.setSize(sf::Vector2f(lay[2], lay[3]));
        shape.setPosition(lay[0], lay[1]);
        
    }
};

int main()
{
    auto layout = create_layout(300, 700);
    layout.run();
    sf::RenderWindow window(sf::VideoMode(300, 700), "SFML");
    window.setView(sf::View(sf::FloatRect(0,0,300,700)));

    ShapeWithLayout<sf::RectangleShape> title_with_layout{layout.title};
    title_with_layout.update(layout);

    ShapeWithLayout<sf::RectangleShape> splits_with_layout{layout.splits};
    splits_with_layout.shape.setFillColor(sf::Color(255,0,0));
    splits_with_layout.update(layout);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Resized)
            {
                window.setView(sf::View(sf::FloatRect(0,0,event.size.width,event.size.height)));
                layout.resize(event.size.width, event.size.height);
                layout.run();
                title_with_layout.update(layout);
                splits_with_layout.update(layout);
            }
        }

        window.clear();
        window.draw(title_with_layout.shape);
        window.draw(splits_with_layout.shape);
        window.display();
    }
}
