/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include "SVGAnimatedListPropertyTearOff.h"
#include "SVGPathSegListValues.h"

namespace WebCore {

class SVGPathElement;

class SVGPathSegList final : public SVGListProperty<SVGPathSegListValues> {
public:
    using Base = SVGListProperty<SVGPathSegListValues>;
    using AnimatedListPropertyTearOff = SVGAnimatedListPropertyTearOff<SVGPathSegListValues>;
    using ListItemType = SVGPropertyTraits<SVGPathSegListValues>::ListItemType;
    using PtrListItemType = RefPtr<SVGPathSeg>;

    static Ref<SVGPathSegList> create(AnimatedListPropertyTearOff& animatedProperty, SVGPropertyRole role, SVGPathSegRole pathSegRole, SVGPathSegListValues& values, ListWrapperCache& wrappers)
    {
        return adoptRef(*new SVGPathSegList(animatedProperty, role, pathSegRole, values, wrappers));
    }

    static Ref<SVGPathSegList> create(AnimatedListPropertyTearOff& animatedProperty, SVGPropertyRole role, SVGPathSegListValues& values, ListWrapperCache& wrappers)
    {
        ASSERT_NOT_REACHED();
        return adoptRef(*new SVGPathSegList(animatedProperty, role, PathSegUndefinedRole, values, wrappers));
    }

    int findItem(const ListItemType& item) const
    {
        ASSERT(m_values);

        unsigned size = m_values->size();
        for (size_t i = 0; i < size; ++i) {
            if (item == m_values->at(i))
                return i;
        }

        return -1;
    }

    void removeItemFromList(size_t itemIndex, bool shouldSynchronizeWrappers)
    {
        ASSERT(m_values);
        ASSERT_WITH_SECURITY_IMPLICATION(itemIndex < m_values->size());

        m_values->remove(itemIndex);

        if (shouldSynchronizeWrappers)
            commitChange();
    }

    // SVGList API
    ExceptionOr<void> clear();

    ExceptionOr<PtrListItemType> initialize(PtrListItemType newItem)
    {
        // Not specified, but FF/Opera do it this way, and it's just sane.
        if (!newItem)
            return Exception { SVGException::SVG_WRONG_TYPE_ERR };

        clearContextAndRoles();
        return Base::initializeValues(newItem);
    }

    ExceptionOr<PtrListItemType> getItem(unsigned index);

    ExceptionOr<PtrListItemType> insertItemBefore(PtrListItemType newItem, unsigned index)
    {
        // Not specified, but FF/Opera do it this way, and it's just sane.
        if (!newItem)
            return Exception { SVGException::SVG_WRONG_TYPE_ERR };

        return Base::insertItemBeforeValues(newItem, index);
    }

    ExceptionOr<PtrListItemType> replaceItem(PtrListItemType, unsigned index);

    ExceptionOr<PtrListItemType> removeItem(unsigned index);

    ExceptionOr<PtrListItemType> appendItem(PtrListItemType newItem)
    {
        // Not specified, but FF/Opera do it this way, and it's just sane.
        if (!newItem)
            return Exception { SVGException::SVG_WRONG_TYPE_ERR };

        return Base::appendItemValues(newItem);
    }

private:
    SVGPathSegList(AnimatedListPropertyTearOff& animatedProperty, SVGPropertyRole role, SVGPathSegRole pathSegRole, SVGPathSegListValues& values, ListWrapperCache& wrappers)
        : SVGListProperty<SVGPathSegListValues>(role, values, &wrappers)
        , m_animatedProperty(&animatedProperty)
        , m_pathSegRole(pathSegRole)
    {
    }

    virtual ~SVGPathSegList()
    {
        if (m_animatedProperty)
            m_animatedProperty->propertyWillBeDeleted(*this);
    }

    SVGPathElement* contextElement() const;

    void clearContextAndRoles();

    using Base::m_role;

    bool isReadOnly() const final
    {
        if (m_role == AnimValRole)
            return true;
        if (m_animatedProperty && m_animatedProperty->isReadOnly())
            return true;
        return false;
    }

    void commitChange() final
    {
        ASSERT(m_values);
        m_values->commitChange(*m_animatedProperty->contextElement(), ListModificationUnknown);
    }

    void commitChange(ListModification listModification) final
    {
        ASSERT(m_values);
        m_values->commitChange(*m_animatedProperty->contextElement(), listModification);
    }

    bool processIncomingListItemValue(const ListItemType& newItem, unsigned* indexToModify) final;
    bool processIncomingListItemWrapper(Ref<ListItemTearOff>&, unsigned*) final
    {
        ASSERT_NOT_REACHED();
        return true;
    }

private:
    RefPtr<AnimatedListPropertyTearOff> m_animatedProperty;
    SVGPathSegRole m_pathSegRole;
};

} // namespace WebCore
