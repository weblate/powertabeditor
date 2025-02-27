/*
  * Copyright (C) 2013 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "position.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

VolumeSwell::VolumeSwell(VolumeLevel start, VolumeLevel end, int duration)
    : myStartVolume(start), myEndVolume(end), myDuration(duration)
{
}

bool
VolumeSwell::operator==(const VolumeSwell &other) const
{
    return myStartVolume == other.myStartVolume &&
           myEndVolume == other.myEndVolume && myDuration == other.myDuration;
}

TremoloBar::TremoloBar(Type type, int pitch, int duration)
    : myType(type), myPitch(pitch), myDuration(duration)
{
}

bool
TremoloBar::operator==(const TremoloBar &other) const
{
    return myType == other.myType && myPitch == other.myPitch &&
           myDuration == other.myDuration;
}

std::string
TremoloBar::getPitchText(int pitch)
{
    if (pitch == 0)
        return "Standard";

    std::ostringstream text;

    // Display a fraction.
    const int quotient = pitch / 4;
    const int remainder = pitch % 4;

    // Handle whole numbers.
    if (quotient != 0)
    {
        text << quotient;

        if (remainder != 0)
            text << " ";
    }

    // Fractional part.
    if (remainder != 0)
    {
        // Reduce the fraction.
        if (remainder == 1 || remainder == 3)
            text << remainder << "/" << 4;
        else
            text << "1/2";
    }

    return text.str();
}

Position::Position()
    : myPosition(0),
      myDurationType(EighthNote),
      myMultiBarRestCount(0)
{
}

Position::Position(int position, DurationType duration)
    : myPosition(position),
      myDurationType(duration),
      myMultiBarRestCount(0)
{
}

bool Position::operator==(const Position &other) const
{
    return myPosition == other.myPosition &&
           myDurationType == other.myDurationType &&
           mySimpleProperties == other.mySimpleProperties &&
           myMultiBarRestCount == other.myMultiBarRestCount &&
           myVolumeSwell == other.myVolumeSwell &&
           myTremoloBar == other.myTremoloBar &&
           myNotes == other.myNotes;
}

int Position::getPosition() const
{
    return myPosition;
}

void Position::setPosition(int position)
{
    myPosition = position;
}

Position::DurationType Position::getDurationType() const
{
    return myDurationType;
}

void Position::setDurationType(DurationType type)
{
    myDurationType = type;
}

bool Position::hasProperty(SimpleProperty property) const
{
    return mySimpleProperties.test(property);
}

void Position::setProperty(SimpleProperty property, bool set)
{
    // Handle any mutually-exclusive properties.
    if (set)
    {
        if (property == PickStrokeUp && hasProperty(PickStrokeDown))
            mySimpleProperties.set(PickStrokeDown, false);
        if (property == PickStrokeDown && hasProperty(PickStrokeUp))
            mySimpleProperties.set(PickStrokeUp, false);

        if (property == Vibrato && hasProperty(WideVibrato))
            mySimpleProperties.set(WideVibrato, false);
        if (property == WideVibrato && hasProperty(Vibrato))
            mySimpleProperties.set(Vibrato, false);

        if (property == ArpeggioUp && hasProperty(ArpeggioDown))
            mySimpleProperties.set(ArpeggioDown, false);
        if (property == ArpeggioDown && hasProperty(ArpeggioUp))
            mySimpleProperties.set(ArpeggioUp, false);

        if (property == Dotted && hasProperty(DoubleDotted))
            mySimpleProperties.set(DoubleDotted, false);
        if (property == DoubleDotted && hasProperty(Dotted))
            mySimpleProperties.set(Dotted, false);

        if (property == Marcato && hasProperty(Sforzando))
            mySimpleProperties.set(Sforzando, false);
        if (property == Sforzando && hasProperty(Marcato))
            mySimpleProperties.set(Marcato, false);

        if (property == TripletFeelFirst && hasProperty(TripletFeelSecond))
            mySimpleProperties.set(TripletFeelSecond, false);
        if (property == TripletFeelSecond && hasProperty(TripletFeelFirst))
            mySimpleProperties.set(TripletFeelFirst, false);
    }

    mySimpleProperties.set(property, set);
}

bool Position::isRest() const
{
    return hasProperty(Rest);
}

void Position::setRest(bool set)
{
    setProperty(Rest, set);

    if (set)
        myNotes.clear();
}

bool Position::hasMultiBarRest() const
{
    return myMultiBarRestCount != 0;
}

int Position::getMultiBarRestCount() const
{
    if (!hasMultiBarRest())
        throw std::logic_error("Position does not have a multi-bar rest");

    return myMultiBarRestCount;
}

void Position::setMultiBarRest(int count)
{
    if (count < 2)
        throw std::out_of_range("Invalid multi-bar rest count");

    myMultiBarRestCount = count;
}

void Position::clearMultiBarRest()
{
    myMultiBarRestCount = 0;
}

bool
Position::hasVolumeSwell() const
{
    return myVolumeSwell.has_value();
}

const VolumeSwell &
Position::getVolumeSwell() const
{
    return *myVolumeSwell;
}

void
Position::setVolumeSwell(const VolumeSwell &swell)
{
    myVolumeSwell = swell;
}

void
Position::clearVolumeSwell()
{
    myVolumeSwell.reset();
}

bool
Position::hasTremoloBar() const
{
    return myTremoloBar.has_value();
}

const TremoloBar &
Position::getTremoloBar() const
{
    return *myTremoloBar;
}

void
Position::setTremoloBar(const TremoloBar &bar)
{
    myTremoloBar = bar;
}

void
Position::clearTremoloBar()
{
    myTremoloBar.reset();
}

boost::iterator_range<Position::NoteIterator> Position::getNotes()
{
    return boost::make_iterator_range(myNotes);
}

boost::iterator_range<Position::NoteConstIterator> Position::getNotes() const
{
    return boost::make_iterator_range(myNotes);
}

void Position::insertNote(const Note &note)
{
    myNotes.push_back(note);

    // Sort notes by string.
    std::sort(myNotes.begin(), myNotes.end(), [](const Note &note1, const Note &note2) {
        return note1.getString() < note2.getString();
    });
}

void Position::removeNote(const Note &note)
{
    removeNotes([=](const Note &other) { return other == note; });
}

const Note *Utils::findByString(const Position &pos, int string)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.getString() == string)
            return &note;
    }

    return nullptr;
}

Note *
Utils::findByString(Position &pos, int string)
{
    return const_cast<Note *>(
        Utils::findByString(const_cast<const Position &>(pos), string));
}

bool Utils::hasNoteWithTappedHarmonic(const Position &pos)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasTappedHarmonic())
            return true;
    }

    return false;
}

bool Utils::hasNoteWithProperty(const Position &pos,
                                Note::SimpleProperty property)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasProperty(property))
            return true;
    }

    return false;
}

bool Utils::hasNoteWithTrill(const Position &pos)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasTrill())
            return true;
    }

    return false;
}

bool Utils::hasNoteWithBend(const Position &pos)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasBend())
            return true;
    }

    return false;
}

bool Utils::hasNoteWithArtificialHarmonic(const Position &pos)
{
    for (const Note &note : pos.getNotes())
    {
        if (note.hasArtificialHarmonic())
            return true;
    }

    return false;
}
