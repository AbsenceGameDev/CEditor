#pragma once
#ifndef EDWIDGETSUI_H
#define EDWIDGETSUI_H


#include "styling.h"
#include "widgets.h"

// fwd decl.
struct Template;

//
// Menus and editors
struct WMainMenu final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
   virtual void operator()() final { Draw(WidgetStyling);}
   static void Draw(CE::Style::Compound& StaticWidgetStyling);

   virtual void OnStateUpdate(bool OldState) final {}
};

struct WClassEditor final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
   virtual void operator()() final { Draw(WidgetStyling);}
   static void Draw(CE::Style::Compound& StaticWidgetStyling);

   virtual void OnStateUpdate(bool OldState) final {}
   
};

struct WRaceEditor final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
   virtual void operator()() final { Draw(WidgetStyling);}
   static void Draw(CE::Style::Compound& StaticWidgetStyling);

   virtual void OnStateUpdate(bool OldState) final {}
   
};

struct WCharacterEditor final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
   virtual void operator()() final { Draw(WidgetStyling);}
   static void Draw(CE::Style::Compound& StaticWidgetStyling);

   virtual void OnStateUpdate(bool OldState) final {}
   
};

struct WCharacterSelector final : public LinkedWidgetBase
{
   SCOPED_USE_LINKEDWIDGET_NAMESPACE
   SCOPED_USE_MAKE_WIDGET_NOT_ABSTRACT
   
   virtual void operator()() final { Draw(WidgetStyling);}
   static void Draw(CE::Style::Compound& StaticWidgetStyling);

   virtual void OnStateUpdate(bool OldState) final {}
};

#endif // EDWIDGETSUI_H
