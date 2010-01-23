package dto.security.client
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.TermInfo")]
	public class TermInfo
	{
			public var  termId:int;
			public var  termType:TermType;
			public var  termName:String;
			public var  termAddr:String;
			public var  termDec:String;
			public var  termIp:String;
			public var  termMask:String; 
			public var  termGateway:String;
			public var  termDns:String;
			public var  termScale:String;
			public var  termResolution:String;
			public var  termScreen:String;
			public var  termRoot:String;
			public var  termNum:String;
			public var  termPlaylists:ArrayCollection;
			public var  targetGroups:ArrayCollection;
			public var 	termActivation:int
			public var  termInitialscreen:String;
			public var  termCom:int;
			
			public function TermInfo()
			{	
			}

	}
}