#include "Lua/LuaComponentBindings.h"
#include "Lua/LuaManager.h"
#include "Scene/Scene.h"
#include "Renderer/Font.h"
#include "Renderer/SpriteRenderer.h"
#include "Renderer/TextRenderer.h"
#include "Scene/Transform.h"
#include "UI/Button.h"

namespace NK{

	void LuaComponentBindings::RegisterAll(LuaManager& lua) {
        // --- Глобальные компоненты ---
         lua.BindClass<Transform>("Transform",
            "SetPosition", [](Transform& t, double x, double y, double z) {
                t.SetPosition(glm::vec3((float)x, (float)y, (float)z));
            },
            "GetPosition", [](Transform& t) -> std::tuple<float, float, float> {
                auto& p = t.GetPosition();
                return { p.x, p.y, p.z };
            },
            "SetRotation", &Transform::SetRotation,
            "GetRotationDegrees", &Transform::GetRotationDegrees,
            "SetScale", [](Transform& t, float x, float y) { t.SetScale(glm::vec2(x, y)); },
            "GetScale", [](Transform& t) -> std::tuple<float, float> {
                auto& s = t.GetScale();
                return { s.x, s.y };
            },
            sol::base_classes, sol::bases<Component>()
        );

        // SpriteRenderer
        lua.BindClass<SpriteRenderer>("SpriteRenderer",
            "SetTexture", &SpriteRenderer::SetTexture,
            "SetShader", &SpriteRenderer::SetShader,
            "SetAlignment", [](SpriteRenderer& sr, double h, double v) { sr.SetAlignment((float)h, (float)v); },
            "SetIsUI", &SpriteRenderer::SetIsUI,
            "SetColor", [](SpriteRenderer& sr, double r, double g, double b, double a) {
                sr.SetColor((float)r, (float)g, (float)b, (float)a);
            },
            "SetUseColor", &SpriteRenderer::SetUseColor,
            sol::base_classes, sol::bases<Component>()
        );

	    // TextRenderer
	    lua.BindClass<TextRenderer>("TextRenderer",
            "SetFont", &TextRenderer::SetFont,
            "SetText", &TextRenderer::SetText,
            "SetFontSize", [](TextRenderer& tr, double size) { tr.SetFontSize((float)size); },
            "SetAlignment", [](TextRenderer& tr, double h, double v) { tr.SetAlignment((float)h, (float)v); },
            "SetColor", [](TextRenderer& tr, int r, int g, int b, int a) {
                tr.SetColor((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
            },
            sol::base_classes, sol::bases<Component>()
        );

	    // Button
	    lua.BindClass<Button>("Button",
            "SetCallback", &Button::SetCallback,
            "SetOnClick", &Button::SetOnClick,
            "SetOnPointerDown", &Button::SetOnPointerDown,
            "SetOnPointerUp", &Button::SetOnPointerDown,
            "SetOnPointerExit", &Button::SetOnPointerExit,
            "SetOnPointerUp", &Button::SetOnPointerUp,
            "SetSize", [](Button& btn, double x, double y) {
                btn.SetSize((float)x, (float)y);
            },
            sol::base_classes, sol::bases<Component>()
        );

	    // Anchor
	    lua.BindClass<Anchor>("Anchor",
            "SetPreset", [](Anchor& a, int preset) { a.SetPreset(static_cast<AnchorPreset>(preset)); },
            "SetScreenAnchor", [](Anchor& a, double sx, double sy) { a.SetScreenAnchor((float)sx, (float)sy); },
            "SetObjectAnchor", [](Anchor& a, double ox, double oy) { a.SetObjectAnchor((float)ox, (float)oy); },
            "SetSize", [](Anchor& a, double w, double h) { a.SetSize(glm::vec2((float)w, (float)h)); },
            sol::base_classes, sol::bases<Component>()
        );
	}
}
