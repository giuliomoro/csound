/* 
 
 SimpleTest2ViewController.h:
 
 Copyright (C) 2014 Steven Yi, Victor Lazzarini, Aurelius Prochazka
 
 This file is part of Csound iOS Examples.
 
 The Csound for iOS Library is free software; you can redistribute it
 and/or modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.   
 
 Csound is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with Csound; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 02111-1307 USA
 
 */

#import "SimpleTest2ViewController.h"

@implementation SimpleTest2ViewController

-(void)viewDidLoad {
    self.title = @"Simple Test 2";
    [super viewDidLoad];
}

-(IBAction) toggleOnOff:(id)component {
	UISwitch *uiswitch = (UISwitch *)component;
	NSLog(@"Status: %d", [uiswitch isOn]);
    
	if(uiswitch.on) {
        
        NSString *tempFile = [[NSBundle mainBundle] pathForResource:@"test2" ofType:@"csd"];  
        NSLog(@"FILE PATH: %@", tempFile);
        
		[self.csound stop];
        
        self.csound = [[CsoundObj alloc] init];
        [self.csound addListener:self];
        
        CsoundUI *csoundUI = [[CsoundUI alloc] initWithCsoundObj:self.csound];
        [csoundUI addSlider:mRateSlider forChannelName:@"noteRate"];
        [csoundUI addSlider:mDurationSlider forChannelName:@"duration"];
        [csoundUI addSlider:mAttackSlider forChannelName:@"attack"];
        [csoundUI addSlider:mDecaySlider forChannelName:@"decay"];
        [csoundUI addSlider:mSustainSlider forChannelName:@"sustain"];
        [csoundUI addSlider:mReleaseSlider forChannelName:@"release"];
        
        [self.csound play:tempFile];
        
	} else {
        [self.csound stop];
    }
}



#pragma mark CsoundObjListener

-(void)csoundObjCompleted:(CsoundObj *)csoundObj {
	[mSwitch setOn:NO animated:YES];
}
@end
