
/// Flank filter utility class

class FlankFilter
{
private:
    bool _lastValue=false;
    bool _flankUp=true;
public:
    FlankFilter(bool flankUp=true);
    ~FlankFilter();
    bool filter(bool currentValue);
};

FlankFilter::FlankFilter(bool flankUpPar) {
    _flankUp = flankUpPar;
}

bool FlankFilter::filter(bool currentValue) {
    
    bool returnValue = false;

    if (_lastValue!=currentValue) {
        if(_lastValue==false & _flankUp) {
            returnValue = true;
        } else if (_lastValue==true & !_flankUp) {
            returnValue = true;
        }
        _lastValue = currentValue;
    }

    return (returnValue);
}

FlankFilter::~FlankFilter()
{
}
