#include "entry.h"

void entry::on_init(context* ctx)
{
    _time_elapsed = 0.0f;
    this->_app_name = "seal2d.hello-world";
    this->_app_version = "v0.0.1";

#if S2D_ENABLE_LUA == 1

    ctx->_lua_context->on_start(ctx, "res/scripts/main.lua");
#else
    rect visible_rect = ctx->get_visible_rect();

    LOGD("entry:: on_init");

    ctx->_sprite_frame_cache->load("res/animations/pirate.json", "res/animations/pirate.png");

    node* root = ctx->get_root();

#if 1
//
//    int j = 0;
//    for (int i = 0; i < 40; ++i) {
//        particle* p = new particle();
//        p->init("res/particles/flower.json");
//        p->set_pos(((i+1)%10) * 100, j*100);
//        if (i%10 == 0) {
//            j++;
//        }
//        root->add_child(p);
//    }

//    for (int i = 0; i < 5; ++i) {
//        spine_anim* s = new spine_anim();
//        s->init("res/spines/unit_skull.json", "res/spines/unit_skull.atlas");
//        s->set_anim("attack_1", 0, -1);
//
//        //    s->init("res/spines/spineboy-pro.json", "res/spines/spineboy.atlas");
//        s->set_pos(200*i + 100, 200);
//        root->add_child(s);
//    }
#else
    root->set_anchor(0, 0);
    root->set_pos(0, 0);

    panel* first = new panel();
    root->add_child(first);

    first->init(rect::make_rect(0, 0, 500, 500));
    first->set_pos(0, 0);
    first->set_anchor(0, 0);

    const char* tests[] = {"attack_0.png", "attack_1.png",
                        "attack_2.png", "attack_3.png",
                        "attack_4.png"};

    for (int i = 0; i < sizeof(tests)/sizeof(const char*); ++i) {
        sprite_frame* f = ctx->_sprite_frame_cache->get(tests[i]);
        sprite* s = new sprite();
        s->init(f);
        s->set_pos( 0 + i*120, 0);
        s->set_anchor(0, 0);
        first->add_child(s);
    }
#endif

#endif
}

void entry::on_pause()
{
    LOGD("entry:: on_pause");
}

void entry::on_resume()
{
    LOGD("entry:: on_resume");
}

void entry::on_destroy()
{
    LOGD("entry:: on_destroy");
}

void entry::on_resize(context* ctx)
{

}

void entry::on_begin_update(float dt)
{

}

void entry::on_after_update(float dt)
{
    _time_elapsed += dt;
    if (_time_elapsed > 2.0f) {
        _time_elapsed -= 2.0f;
        util::profile_stats();
    }
}
