#include "pch.h"
#include "TSpanView.h"
#include "TSpanView.g.cpp"

#include <winrt/Microsoft.Graphics.Canvas.Text.h>
#include "Utils.h"

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
   auto const &resourceCreator{canvas.try_as<ICanvasResourceCreator>()};
   Microsoft::Graphics::Canvas::Text::CanvasTextFormat const &textFormat{};
   textFormat.FontSize(FontSize());
   textFormat.FontFamily(FontFamily());
   textFormat.FontWeight(Utils::FontWeightFrom(FontWeight(), SvgParent()));

   //UPDATED IMPLEMENTATION
   auto const &testBrush{Brushes::CanvasSolidColorBrush(resourceCreator, Colors::Red())};
   session.DrawText(to_hstring(m_content), 10.0f, 10.0f, canvas.Size().Width, canvas.Size().Height, testBrush, textFormat);

  // OLD IMPLEMENTATION
  Geometry(Geometry::CanvasGeometry::CreateText(
      {resourceCreator, to_hstring(m_content), textFormat, canvas.Size().Width, canvas.Size().Height}));
}

void TSpanView::Render(UI::Xaml::CanvasControl const &canvas, CanvasDrawingSession const &session) {
  auto const &resourceCreator{canvas.try_as<ICanvasResourceCreator>()};

  CreateGeometry(canvas, session);

  auto geometry{Geometry()};
  if (m_propSetMap[RNSVG::BaseProp::Matrix]) {
    geometry = geometry.Transform(SvgTransform());
  }

  geometry = Geometry::CanvasGeometry::CreateGroup(resourceCreator, {geometry}, FillRule());

  if (auto const &opacityLayer{session.CreateLayer(m_opacity)}) {
    if (auto const &fillLayer{session.CreateLayer(FillOpacity())}) {
      auto const &fill{Utils::GetCanvasBrush(FillBrushId(), Fill(), SvgRoot(), geometry, resourceCreator)};
      session.FillGeometry(geometry, fill);
      fillLayer.Close();
    }

    if (auto const &strokeLayer{session.CreateLayer(StrokeOpacity())}) {
      Geometry::CanvasStrokeStyle strokeStyle{};
      strokeStyle.StartCap(StrokeLineCap());
      strokeStyle.EndCap(StrokeLineCap());
      strokeStyle.LineJoin(StrokeLineJoin());
      strokeStyle.DashOffset(StrokeDashOffset());
      strokeStyle.MiterLimit(StrokeMiterLimit());

      float canvasDiagonal{Utils::GetCanvasDiagonal(canvas.Size())};
      float strokeWidth{Utils::GetAbsoluteLength(StrokeWidth(), canvasDiagonal)};
      strokeStyle.CustomDashStyle(Utils::GetAdjustedStrokeArray(StrokeDashArray(), strokeWidth, canvasDiagonal));

      auto const &stroke{Utils::GetCanvasBrush(StrokeBrushId(), Stroke(), SvgRoot(), geometry, resourceCreator)};
      session.DrawGeometry(geometry, stroke, strokeWidth, strokeStyle);
      strokeLayer.Close();
    }

    opacityLayer.Close();
  }
}
} // namespace winrt::RNSVG::implementation
