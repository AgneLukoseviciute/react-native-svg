#include "pch.h"
#include "TSpanView.h"
#include "TSpanView.g.cpp"

#include "Utils.h"
#include <winrt/Microsoft.Graphics.Canvas.Text.h>

using namespace winrt;
using namespace Microsoft::Graphics::Canvas;
using namespace Microsoft::ReactNative;

namespace winrt::RNSVG::implementation {
void TSpanView::UpdateProperties(IJSValueReader const &reader, bool forceUpdate, bool invalidate) {
  const JSValueObject &propertyMap{JSValue::ReadObjectFrom(reader)};

  for (auto const &pair : propertyMap) {
    auto const &propertyName{pair.first};
    auto const &propertyValue{pair.second};

    if (propertyName == "content") {
      m_content = propertyValue.AsString();
    }
  }

  __super::UpdateProperties(reader, forceUpdate, invalidate);
}

void TSpanView::CreateGeometry(UI::Xaml::CanvasControl const &canvas, CanvasDrawingSession const &session) {
 /* //UPDATED IMPLEMENTATION
  
  Microsoft::Graphics::Canvas::Text::CanvasTextFormat const &textFormat{};
  auto const &resourceCreator{canvas.try_as<ICanvasResourceCreator>()};
  auto geometry{Geometry()};
  textFormat.FontSize(FontSize());
  textFormat.FontFamily(FontFamily());
  textFormat.FontWeight(Utils::FontWeightFrom(FontWeight(), SvgParent()));
  auto const &testBrush{Utils::GetCanvasBrush(StrokeBrushId(), Stroke(), SvgRoot(), geometry, resourceCreator)};

  session.DrawText(
      to_hstring("test-string"), 1.0, 1.0, canvas.Size().Width, canvas.Size().Height, testBrush, textFormat);*/

  //OLD IMPLEMENTATION
  auto const &resourceCreator{canvas.try_as<ICanvasResourceCreator>()};
  Microsoft::Graphics::Canvas::Text::CanvasTextFormat const &textFormat{};
  textFormat.FontSize(FontSize());
  textFormat.FontFamily(FontFamily());
  textFormat.FontWeight(Utils::FontWeightFrom(FontWeight(), SvgParent()));

  Geometry(Geometry::CanvasGeometry::CreateText(
      {resourceCreator, to_hstring(m_content), textFormat, canvas.Size().Width, canvas.Size().Height}));
}

void TSpanView::Render(UI::Xaml::CanvasControl const &canvas, CanvasDrawingSession const &session) {
  auto const &transform{session.Transform()};

  if (m_propSetMap[RNSVG::BaseProp::Matrix]) {
    session.Transform(transform * SvgTransform());
  }

  if (auto const &opacityLayer{session.CreateLayer(m_opacity)}) {
    CreateGeometry(canvas, session);

    opacityLayer.Close();
  }
  session.Transform(transform);
}
} // namespace winrt::RNSVG::implementation
