#pragma once

namespace Zafkiel
{

struct Visibility
{
  public:

	bool AreChildrenHitTestVisible() const
	{
		return EnumHasAnyFlags(value, VisibilityEnum::ChildrenHitTestVisible);
	}

	bool IsHitTestVisible() const
	{
		return EnumHasAnyFlags(value, VisibilityEnum::SelfHitTestVisible);
	}

	bool IsVisible() const
	{
		return EnumHasAnyFlags(value, VisibilityEnum::Visible | VisibilityEnum::SelfHitTestVisible | VisibilityEnum::ChildrenHitTestVisible);
	}

	/** Visible and hit-testable (can interact with cursor). Default value. */
	static const Visibility Visible;

	/** Not visible and takes up no space in the layout (obviously not hit-testable). */
	static const Visibility Collapsed;

	/** Not visible but occupies layout space (obviously not hit-testable). */
	static const Visibility Hidden;

	/** Visible but not hit-testable (cannot interact with cursor) and children in the hierarchy (if any) are also not hit-testable. */
	static const Visibility HitTestInvisible;

	/** Visible but not hit-testable (cannot interact with cursor) and doesn't affect hit-testing on children (if any). */
	static const Visibility SelfHitTestInvisible;

	/** Any visibility will do */
	static const Visibility All;

  private:

	enum class VisibilityEnum
	{
		Visible = 1 << 0,
		Collapsed = 1 << 1,
		Hidden = 1 << 2,
		SelfHitTestVisible = 1 << 3,
		ChildrenHitTestVisible = 1 << 4,
	};
	ENUM_CLASS_FLAGS_PRIVATE(VisibilityEnum)

    Visibility(VisibilityEnum value) : value(value) {}

	VisibilityEnum value;

};

}
